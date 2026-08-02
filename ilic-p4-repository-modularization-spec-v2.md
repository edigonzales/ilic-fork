# P4-Spezifikation – Revision 2: Plattformneutraler Repository-Kern, stabile `@ilic/tools`-Fassade und direkte Workspace-Integration

**Projekt:** `edigonzales/ilic-fork`  
**Adressat:** autonom arbeitender LLM-Coding-Agent  
**Status:** normative Umsetzungsspezifikation  
**Priorität:** P4  
**Revision:** 2 – verbindlicher Consumer-Kompatibilitätsvertrag  
**Ersetzt:** `ilic-p4-repository-modularization-spec.md`  
**Voraussetzungen:** P0, P1, P2 und P3  
**Referenzstand bei Erstellung:** `9cd3d39aef898cb506151ecebec4316929558337`  
**Normative Sprache:** **MUSS**, **DARF NICHT**, **SOLL**, **SOLLTE**, **KANN**

> **Zentrale Korrektur dieser Revision:** Das neue Paket `@ilic/repository-core` ist eine interne beziehungsweise additive Low-Level-API. Es ersetzt nicht die bestehende öffentliche Klasse `RepositoryManager` aus `@ilic/tools`. `@ilic/tools` MUSS eine echte rückwärtskompatible Fassade implementieren, die bestehende Konstruktoroptionen, Defaults, Imports und Rückgabetypen erhält. Ein bloßer Re-export des Core-Managers ist ausdrücklich unzulässig.

---

## 1. Auftrag in einem Satz

P4 zerlegt die INTERLIS-Repository-Auflösung in einen klar abgegrenzten, deterministischen Repository-Kern und explizite Plattformports für Transport, Cache, Zeit, XML-Decodierung, Prüfsummen und lokale Materialisierung; Native, Node und Browser müssen denselben fachlichen Vertrag erfüllen, und aufgelöste Modellquellen müssen ohne erzwungene temporäre Datei direkt in `SourceManager`, `CompilationSourceStore`, `IliFileCatalog` und `CompilerSession` gelangen.

---

## 2. Verbindlicher Charakter

Diese Datei ist die normative Grundlage der Umsetzung.

Der Coding-Agent MUSS:

1. diese Datei vollständig lesen;
2. den aktuellen Repositorystand vollständig untersuchen;
3. Abweichungen zwischen dieser Spezifikation und dem inzwischen weiterentwickelten Code erkennen;
4. die fachlichen und architektonischen Ziele dieser Spezifikation auf den tatsächlichen Stand übertragen;
5. P0, P1, P2 und P3 erhalten;
6. alle geforderten Tests tatsächlich ausführen;
7. auftretende Fehler analysieren und beheben;
8. den Auftrag nicht bei einem Entwurf, Teilgerüst oder nur grünen Unit-Tests beenden.

Beispielcode in dieser Spezifikation ist normativ hinsichtlich Verantwortung, Lebensdauer, Richtung der Abhängigkeiten und Fehlersemantik. Namen dürfen nur dann abweichen, wenn der bestehende Projektstil oder der aktuelle Code dafür einen klaren Grund liefert. Eine bloße persönliche Präferenz ist kein Grund.

---

## 3. Ausgangslage

Beim Referenzstand existieren zwei Repository-Implementierungen:

### 3.1 Native C++-Implementierung

Die native Implementierung besteht unter anderem aus:

- `include/ilic/Repository.h`
- `source/repository/Repository.cpp`
- `source/repository/RepositoryUri.*`
- `source/repository/RepositoryXml.*`
- `source/repository/RepositoryVersion.*`
- `source/repository/RepositoryCrawler.*`
- `source/repository/RepositoryTransport.*`
- `source/repository/RepositoryCache.*`
- `source/repository/RepositoryResourceLoader.*`
- `source/main/RepositoryCli.*`

Sie wird nur gebaut, wenn:

```cmake
ILIC_ENABLE_NATIVE_REPOSITORY=ON
```

und der Build kein Emscripten-Build ist.

Die native Fassade `ilic::RepositoryManager` besitzt derzeit indirekt:

- `CurlRepositoryTransport`;
- einen dateibasierten `RepositoryCache`;
- einen `RepositoryResourceLoader`;
- einen `RepositoryCrawler`;
- In-Memory-Zustand für geladene `ilimodels.xml`;
- In-Memory-Zustand für geladene `ilisite.xml`;
- Abhängigkeitsauflösung;
- Prüfsummenprüfung;
- temporäre Materialisierung von Modellen.

### 3.2 JavaScript-Implementierung

`packages/tools/repository-manager.js` enthält derzeit in einer Datei:

- XML-Entity-Decodierung;
- reguläre Ausdrücke für `ilimodels.xml`;
- reguläre Ausdrücke für `ilisite.xml`;
- einen eigenen MD5-Algorithmus;
- Cache-/TTL-Entscheidungen;
- Offline-Verhalten;
- Stale-on-error-Verhalten;
- Fetch;
- Site-Graph-Traversierung;
- Modellversionswahl;
- Abhängigkeitsauflösung;
- Pfadvalidierung;
- Workspace-Erzeugung.

Node und Browser stellen zusätzliche Cacheklassen bereit:

- `NodeFileCache`;
- `BrowserCache`;
- `MemoryCache`.

### 3.3 Compilerintegration

Der native CLI-Pfad lädt aufgelöste Modelle derzeit über `ResolvedModel.localPath` wieder als Datei in den `IliFileCatalog`.

Das ist unnötig, weil `ResolvedModel` bereits `source` und `uri` enthält. Es führt zu folgenden Problemen:

- Downloadinhalt wird zunächst im Speicher gehalten;
- danach in Cache oder temporäre Datei geschrieben;
- anschließend erneut aus dem Dateisystem gelesen;
- der Compiler hängt funktional von einer lokalen Datei ab;
- ein fehlgeschlagener Cache-Write kann eine ansonsten erfolgreiche Kompilierung verhindern;
- Browser und Native verwenden unterschiedliche Integrationswege;
- reine In-Memory-Repositories sind unnötig schwierig;
- Tests prüfen nicht zuverlässig, dass die übergebenen Bytes exakt die kompilierten Bytes sind.

### 3.4 P2- und P3-Kontext

P2 hat bereits:

- pro Lauf einen `CompilerContext`;
- eindeutige Ownership;
- `CompilationSourceStore`;
- `IliFileCatalog`;
- sessionlokale Quellen;
- keine globalen Compilerzustände;
- parallele unabhängige Sessions.

P3 führt beziehungsweise soll einführen:

- einen einzigen autoritativen C++-/ANTLR-Parser;
- Native/WASM-Snapshot-Parität;
- einen dünnen JavaScript-Wrapper;
- unveränderte High-Level-Consumer-APIs.

P4 DARF diese Errungenschaften nicht zurücknehmen.

---

## 4. Problemdefinition

P4 löst fünf zusammenhängende Architekturprobleme.

### 4.1 Vermischte Verantwortungen

Transport, Cache, Zeit, XML, Repositorygraph, Versionierung, Abhängigkeiten und Compilerinstallation sind heute nicht ausreichend getrennt.

### 4.2 Plattformabhängigkeiten im falschen Layer

Der fachliche Repositoryalgorithmus kennt heute teilweise:

- `fetch`;
- `Date.now()`;
- `indexedDB`;
- Node-Dateisystem;
- cURL;
- `std::filesystem`;
- Cachedateien;
- temporäre Dateien.

### 4.3 Native/JavaScript-Drift

Native und JavaScript lösen dieselben fachlichen Aufgaben unabhängig. Dadurch können sich unterscheiden:

- URI-Normalisierung;
- XML-Akzeptanz;
- Site-Traversierung;
- Versionswahl;
- Warnungen;
- Abhängigkeitsreihenfolge;
- Fehlercodes;
- Prüfsummenverhalten;
- Stale-/Offline-Verhalten.

### 4.4 Erzwungene Materialisierung

Der Compiler verwendet im nativen Repositorypfad die lokale Datei statt der bereits aufgelösten Source.

### 4.5 Unzureichend präziser Cachevertrag

Die öffentliche JavaScript-Schnittstelle und die native Implementierung definieren nicht ausreichend exakt:

- wer `storedAt` setzt;
- welche Zeitquelle gilt;
- wann ein Eintrag frisch ist;
- welche Fehler einen Stale-Fallback erlauben;
- wie konkurrierende Writes behandelt werden;
- wie Cachemigration funktioniert;
- ob Cachefehler fatal sind;
- wie eine Prüfsummenabweichung behandelt wird.

---

## 5. Ziele

P4 MUSS alle folgenden Ziele erreichen.

### 5.1 Repository-Kern

Es MUSS einen klar benannten Repository-Kern geben, der:

- keine Netzwerkverbindung selbst öffnet;
- keine Cachedatei selbst schreibt;
- keine Browserdatenbank selbst öffnet;
- keine Systemzeit direkt liest;
- keine CLI-Ausgabe erzeugt;
- keinen Compilerprozess beendet;
- keine globale Mutable State verwendet;
- deterministische Fachentscheidungen trifft;
- durch Fake-Ports vollständig testbar ist.

### 5.2 Explizite Ports

Mindestens folgende Abhängigkeiten MÜSSEN explizit injiziert werden:

- Transport;
- Cache;
- Uhr;
- Metadaten-/XML-Decoder;
- Prüfsummenberechnung;
- optional Cancellation;
- optional Ereignis-/Diagnosesink.

### 5.3 Plattformadapter

Es MUSS getrennte Adapter geben für:

- Native HTTP;
- Native lokale Dateien;
- Native Disk-Cache;
- Node HTTP/Fetch;
- Node File-Cache;
- Browser Fetch;
- Browser IndexedDB;
- Memory-Cache;
- deterministische Testuhr;
- Fake-Transport.

### 5.4 Direkte Compilerintegration

`ResolvedModel.source` MUSS die autoritative Compilerquelle sein.

Eine lokale Datei darf:

- als Cacheartefakt;
- als Debug-/Provenienzangabe;
- als optionale Kompatibilitätsinformation

existieren, aber sie DARF NICHT Voraussetzung der Kompilierung sein.

### 5.5 Fachliche Parität

Native und JavaScript MÜSSEN denselben normativen Repositoryvertrag erfüllen.

P4 verlangt nicht zwingend, dass C++ und JavaScript dieselbe Binärimplementierung ausführen. Es verlangt jedoch:

- gemeinsame Contract-Fixtures;
- canonical JSON;
- deterministische Resultate;
- strukturelle Parität für dieselben Szenarien;
- einen CI-Test, der Drift erkennt.

### 5.6 Verbindliche Consumer-Kompatibilität

Bestehende High-Level-Nutzer von `@ilic/tools`, `@ilic/compiler-wasm`, Node-LSP und `interlis-web-ide` DÜRFEN durch P4 nicht zu einer fachlichen Produktionscodeänderung gezwungen werden.

Insbesondere MÜSSEN unverändert nutzbar bleiben:

```ts
import { RepositoryManager, MemoryCache } from "@ilic/tools";
import { NodeFileCache } from "@ilic/tools/node";
import { BrowserCache } from "@ilic/tools/browser";

const manager = new RepositoryManager(options);
const workspace = await manager.resolveWorkspace(models, schemaLanguage);
session.putWorkspace(workspace);
```

Der Kompatibilitätsvertrag umfasst mindestens:

- den Paketpfad `@ilic/tools`;
- den Exportnamen `RepositoryManager`;
- den Exportnamen `MemoryCache`;
- die Subpath-Exports `@ilic/tools/node` und `@ilic/tools/browser`;
- die bestehenden Konstruktoroptionen;
- die bestehende `load`-Callback-Option;
- die Methoden `listModels()`, `resolveModel()` und `resolveWorkspace()`;
- die bestehende Form von `ResolvedWorkspace`;
- die Felder `metadata`, `uri`, `source`, `fromCache` und `stale`;
- das bestehende `onWarning`-Verhalten;
- `CompilerSession.putWorkspace(workspace)`;
- den synchronen P3-Vertrag von `editorSnapshot()` innerhalb einer initialisierten Session.

Neue APIs, Ports, Factories und Ergebnisfelder DÜRFEN nur additiv eingeführt werden. Neue Felder in bestehenden TypeScript-Strukturen MÜSSEN zunächst optional sein, sofern alte Fixtures oder manuell konstruierte Objekte sonst nicht mehr kompilieren würden.

P4 DARF Consumer-Paketversionen und Lockfiles für einen Integrationslauf aktualisieren. P4 DARF jedoch nicht den Produktionscode eines Consumers ändern, um eine Regression in `@ilic/tools` zu kaschieren.

### 5.7 Rollen der JavaScript-Pakete

Die Rollen sind verbindlich getrennt:

- `@ilic/repository-core` stellt eine neue Low-Level-API ohne Plattformdefaults bereit;
- `@ilic/tools` stellt die bestehende High-Level-Kompatibilitäts-API bereit;
- `@ilic/tools/node` stellt Node-spezifische Adapter und Factories bereit;
- `@ilic/tools/browser` stellt Browser-spezifische Adapter und Factories bereit;
- `@ilic/compiler-wasm` konsumiert nur den aufgelösten Workspace und bleibt repositorytransportfrei.

Der öffentliche `RepositoryManager` von `@ilic/tools` MUSS eine Fassade sein. Er DARF NICHT lediglich als Alias oder Re-export von `RepositoryManagerCore` implementiert werden.

---

## 6. Nichtziele

P4 ist ausdrücklich nicht:

- P5 beziehungsweise inkrementelle Kompilierung;
- ein allgemeiner Package Manager;
- ein Maven-/npm-ähnlicher Dependency Resolver;
- ein Ersatz des INTERLIS-Repositoryformats;
- eine Änderung der INTERLIS-Sprache;
- eine Aufhebung der P1-Conformance-Baseline;
- eine neue Authentifizierungsplattform;
- ein Secret Store;
- ein Hintergrunddaemon;
- ein verteiltes Cachecluster;
- eine automatische Veröffentlichung von Modellen;
- eine generelle HTTP-Bibliothek;
- eine Änderung der synchronen Compiler-WASM-Session-API;
- eine Einführung von Netzwerkzugriff im WASM-Compilerkern;
- eine Anhebung über C++17.

Folgende Erweiterungen können später folgen, sind aber für P4 nicht erforderlich:

- ETag-/Last-Modified-Revalidation;
- SHA-256 als neues Metadatenfeld;
- signierte Repositoryindizes;
- Proxy-Authentifizierung;
- Credential Provider;
- inkrementelle Repositoryindizes;
- parallele Downloads mehrerer Modelle;
- persistent gespeicherte Resolution Plans.

---

## 7. Normative Architektur

### 7.1 Abhängigkeitsrichtung

Die Zielrichtung lautet:

```text
Compiler core
    ^
    |
RepositoryWorkspaceInstaller
    ^
    |
Native C++ RepositoryManager facade
    ^
    |
C++ repository core  <--- injected C++ ports <--- native/test adapters

@ilic/compiler-wasm
    ^
    | ResolvedWorkspace via putWorkspace()
    |
@ilic/tools public compatibility facade
    ^                         ^
    |                         |
@ilic/repository-core     @ilic/tools/node + @ilic/tools/browser
(low-level core)          (platform adapters/factories)
```

Präziser:

```text
+--------------------------------------------------------------+
| Consumer applications                                        |
| Node-LSP / Language Service / interlis-web-ide                |
+-------------------------------^------------------------------+
                                | existing @ilic/tools API
+-------------------------------+------------------------------+
| @ilic/tools compatibility facade                             |
| RepositoryManager / MemoryCache / legacy options / defaults  |
+-------------------^---------------------------^--------------+
                    |                           |
        +-----------+-----------+   +-----------+-------------+
        | @ilic/repository-core |   | platform subpath APIs   |
        | RepositoryManagerCore |   | node / browser adapters |
        | pure contract logic   |   | factories and caches    |
        +-----------^-----------+   +-----------^-------------+
                    | injected ports               |
                    +------------------------------+

+--------------------------------------------------------------+
| Native compiler integration                                  |
| Native RepositoryManager facade -> repository C++ core       |
| -> RepositoryWorkspaceInstaller -> CompilerSession sources   |
+--------------------------------------------------------------+
```

`@ilic/tools` hängt vom Core und von plattformneutral verwendbaren Compatibility-Adaptern ab. Der Core hängt niemals von `@ilic/tools` ab. Node-spezifischer Code darf nicht statisch in das Browser-Entry-Point-Bundle gelangen; Browser-spezifischer Code darf nicht Voraussetzung des Node-Entry-Points sein.

### 7.2 Verbotene Abhängigkeiten

`ilic-core` DARF NICHT von der Repositorybibliothek abhängen.

Der Repository-Compiler-Bridge-Layer darf von beiden abhängen:

- `ilic-core`;
- Repository-Fassade beziehungsweise Repository-Datentypen.

Der Repository-Kern DARF NICHT abhängen von:

- CLI;
- `CompilerContext`;
- `IliFileCatalog`;
- cURL;
- Node-Modulen;
- Browserglobals;
- einer konkreten Cacheimplementierung.

### 7.3 Zwei Laufzeitmodelle

Native C++ darf synchron arbeiten.

JavaScript arbeitet asynchron.

Die Fachlogik muss trotzdem dieselben Zustandsübergänge und Entscheidungen abbilden.

Es ist nicht erforderlich, C++-Futures oder JavaScript-Synchronität zu erzwingen.

---

## 8. Vorgeschlagene Zielstruktur

Der Agent SOLL sich an folgender Struktur orientieren:

```text
include/ilic/
  Repository.h
  RepositoryContract.h
  RepositoryCompilerBridge.h

source/repository/core/
  RepositoryPolicy.h
  RepositoryUri.h
  RepositoryUri.cpp
  RepositoryTypes.h
  RepositoryDiagnostics.h
  RepositoryVersionSelector.h
  RepositoryVersionSelector.cpp
  RepositoryCatalog.h
  RepositoryCatalog.cpp
  RepositoryGraphWalker.h
  RepositoryGraphWalker.cpp
  RepositoryDependencyResolver.h
  RepositoryDependencyResolver.cpp
  RepositoryResourceCoordinator.h
  RepositoryResourceCoordinator.cpp
  RepositoryManagerCore.h
  RepositoryManagerCore.cpp

source/repository/ports/
  RepositoryTransport.h
  RepositoryCachePort.h
  RepositoryClock.h
  RepositoryMetadataDecoder.h
  RepositoryChecksum.h
  RepositoryObserver.h

source/repository/native/
  CurlRepositoryTransport.h
  CurlRepositoryTransport.cpp
  LocalFileRepositoryTransport.h
  LocalFileRepositoryTransport.cpp
  FileRepositoryCache.h
  FileRepositoryCache.cpp
  SystemRepositoryClock.h
  LibXmlRepositoryMetadataDecoder.h
  LibXmlRepositoryMetadataDecoder.cpp
  NativeMd5Checksum.h
  NativeRepositoryManager.cpp
  RepositoryWorkspaceInstaller.cpp

packages/repository-core/
  package.json
  index.js
  index.d.ts
  repository-manager-core.js
  resource-coordinator.js
  repository-catalog.js
  repository-graph-walker.js
  dependency-resolver.js
  version-selector.js
  repository-uri.js
  diagnostics.js
  contract.js

packages/tools/
  package.json
  index.js
  index.d.ts
  node.js
  node.d.ts
  browser.js
  browser.d.ts
  compatibility.js
  compatibility-manager.js
  compatibility-memory-cache.js

test/repository/
  contract/
  core/
  native/
  integration/
  compiler/

scripts/
  compare-repository-contract.mjs
  check-repository-layering.py
```

Diese Struktur ist ein Zielbild. Bestehende sinnvolle Dateien können weiterverwendet werden. Der Agent DARF nicht nur umbenennen und die Vermischung behalten.

---

## 9. Repository-Datenvertrag

### 9.1 `ModelMetadata`

Die bestehende öffentliche Semantik MUSS erhalten bleiben:

```cpp
struct ModelMetadata {
    std::string name;
    std::string schemaLanguage;
    std::string file;
    std::string version;
    std::string publishingDate;
    std::string precursorVersion;
    std::string md5;
    std::string repository;
    std::vector<std::string> dependencies;
    bool browseOnly = false;
};
```

In JavaScript:

```ts
export interface ModelMetadata {
  name: string;
  schemaLanguage: string;
  file: string;
  version: string;
  publishingDate: string;
  precursorVersion: string;
  md5: string;
  repository: string;
  dependencies: string[];
  browseOnly: boolean;
}
```

### 9.2 Invarianten

Für jedes `ModelMetadata` gilt:

- `name` darf nicht leer sein;
- `schemaLanguage` darf nicht leer sein;
- `file` darf nicht leer sein;
- `repository` muss normalisiert sein;
- `dependencies` müssen in Dokumentreihenfolge erhalten bleiben;
- leere optionale Strings bleiben leer und werden nicht als `null` exponiert;
- `browseOnly` wird exakt aus dem Metadatenformat abgeleitet;
- unbekannte XML-Felder werden ignoriert;
- doppelte `dependsOnModel`-Werte werden in der eingelesenen Metadatenansicht nicht still entfernt;
- Deduplizierung erfolgt erst im Dependency Resolver.

### 9.3 `ResolvedModel`

Die Source ist autoritativ:

```cpp
struct ResolvedModel {
    ModelMetadata metadata;
    std::string uri;
    std::filesystem::path localPath; // darf leer sein
    std::string source;              // autoritative Bytes als UTF-8-String
    bool fromCache = false;
    bool stale = false;
};
```

Normativ:

- `source` MUSS die tatsächlich validierte und kompilierte Source enthalten;
- `uri` MUSS der kanonische Source-Identifier sein;
- `localPath` KANN leer sein;
- `localPath` DARF NICHT zur Rekonstruktion von `source` verwendet werden;
- `source` und `localPath` dürfen nie unterschiedliche Inhalte repräsentieren;
- Prüfsummen werden über die Bytes von `source` berechnet;
- ein Cache-Write-Fehler darf `source` nicht ungültig machen.

### 9.4 `ResolvedWorkspace`

```ts
export interface ResolvedWorkspace {
  models: Array<{
    metadata: ModelMetadata;
    uri: string;
    source: string;
    fromCache: boolean;
    stale: boolean;
  }>;
}
```

Bestehende Felder dürfen nicht entfernt werden.

Additive optionale Felder sind erlaubt, etwa:

```ts
provenance?: {
  repository: string;
  cacheKey: string;
  resolvedAt: number;
};
```

Solche Felder dürfen die Compilerintegration nicht beeinflussen.

### 9.5 Reihenfolge

`ResolvedWorkspace.models` MUSS deterministisch sein.

Die Reihenfolge lautet:

1. Abhängigkeiten vor abhängigen Modellen;
2. bei mehreren Wurzeln Wurzelreihenfolge aus dem Request;
3. innerhalb einer Dependency-Liste Dokumentreihenfolge;
4. dieselbe Datei nur einmal;
5. ein Modellname nur einmal erfolgreich aufgelöst;
6. keine Hash-Map-Iterationsreihenfolge.

---

## 10. Repository-Request und Policy

### 10.1 `RepositoryResolutionRequest`

C++:

```cpp
struct RepositoryResolutionRequest {
    std::vector<std::string> models;
    std::string schemaLanguage;
};
```

JavaScript:

```ts
export interface RepositoryResolutionRequest {
  models: string[];
  schemaLanguage?: string;
  signal?: AbortSignal;
}
```

### 10.2 `RepositoryPolicy`

Der Kern SOLL eine normalisierte Policy verwenden:

```cpp
struct RepositoryPolicy {
    std::chrono::milliseconds metadataTtl;
    std::chrono::milliseconds modelTtl;
    bool offline;
    bool allowStaleOnError;
    bool followSiteLinks;
    bool validateChecksums;
    std::size_t maxMetadataBytes;
    std::size_t maxModelBytes;
    std::size_t maxRepositoriesVisited;
    std::size_t maxDependencyDepth;
    std::size_t maxModelsResolved;
    std::size_t maxRedirects;
};
```

Empfohlene Defaultwerte:

- metadata TTL: 24 Stunden;
- model TTL: 7 Tage;
- max metadata: 32 MiB;
- max model: 64 MiB;
- max repositories visited: 4096;
- max dependency depth: 1024;
- max resolved models: 10000;
- max redirects: 10.

Die exakten Limits dürfen anhand bestehender Modelle angepasst werden. Sie müssen:

- dokumentiert;
- testbar;
- konfigurierbar;
- in Native und JavaScript äquivalent

sein.

### 10.3 Policy-Normalisierung

Eine Funktion MUSS ungültige Werte normalisieren oder kontrolliert ablehnen:

```cpp
RepositoryPolicy normalizeRepositoryPolicy(
    const RepositoryOptions& options,
    std::vector<Diagnostic>& diagnostics
);
```

Nicht zulässig:

- negative TTL;
- Null-Limits, die versehentlich alles blockieren;
- Integerüberlauf bei Millisekundenumrechnung;
- stilles Abschalten von Prüfsummen;
- unterschiedliche Defaults zwischen Native und Browser.

---

## 11. Port: Uhr

### 11.1 C++-Schnittstelle

```cpp
class RepositoryClock {
public:
    virtual ~RepositoryClock() = default;
    virtual std::chrono::system_clock::time_point now() const = 0;
};
```

Adapter:

```cpp
class SystemRepositoryClock final : public RepositoryClock {
public:
    std::chrono::system_clock::time_point now() const override;
};
```

Testadapter:

```cpp
class ManualRepositoryClock final : public RepositoryClock {
public:
    explicit ManualRepositoryClock(time_point initial);
    time_point now() const override;
    void advance(milliseconds duration);
    void set(time_point value);
};
```

### 11.2 JavaScript-Schnittstelle

```ts
export interface RepositoryClock {
  now(): number; // Unix epoch milliseconds
}
```

### 11.3 Regeln

Der Repository-Kern DARF NICHT direkt verwenden:

- `Date.now()`;
- `new Date()`;
- `std::chrono::system_clock::now()`;
- Dateimtime als fachliche Zeitquelle.

Cacheadapter dürfen Dateisystemmetadaten lesen, aber die Frischeentscheidung trifft der Coordinator anhand der injizierten Uhr.

---

## 12. Port: Transport

### 12.1 C++-Schnittstelle

```cpp
enum class RepositoryResourceKind {
    ModelIndex,
    SiteIndex,
    Model
};

struct RepositoryTransportRequest {
    std::string uri;
    RepositoryResourceKind kind;
    std::size_t maxBytes = 0;
    std::size_t maxRedirects = 0;
};

struct RepositoryTransportResponse {
    bool success = false;
    long statusCode = 0;
    std::string body;
    std::string finalUri;
    std::string error;
    bool notFound = false;
    bool retryable = false;
};

class RepositoryTransport {
public:
    virtual ~RepositoryTransport() = default;
    virtual RepositoryTransportResponse get(
        const RepositoryTransportRequest& request
    ) = 0;
};
```

### 12.2 JavaScript-Schnittstelle

```ts
export interface RepositoryTransportRequest {
  uri: string;
  kind: "model-index" | "site-index" | "model";
  maxBytes: number;
  maxRedirects: number;
  signal?: AbortSignal;
}

export interface RepositoryTransportResponse {
  success: boolean;
  statusCode: number;
  body: Uint8Array;
  finalUri: string;
  error: string;
  notFound: boolean;
  retryable: boolean;
}

export interface RepositoryTransport {
  get(request: RepositoryTransportRequest):
    Promise<RepositoryTransportResponse>;
}
```

### 12.3 Semantik

Der Transport:

- lädt Bytes;
- klassifiziert Statuscodes;
- begrenzt Größe;
- beachtet Cancellation;
- normalisiert keine Repositoryfachdaten;
- parst kein XML;
- wählt keine Modellversion;
- schreibt keinen Cache;
- entscheidet nicht über Stale-Fallback;
- validiert keine Modell-MD5;
- protokolliert keine Secrets.

### 12.4 `CurlRepositoryTransport`

MUSS:

- HTTP und HTTPS unterstützen;
- Redirectlimits beachten;
- final URI liefern;
- TLS-Fehler als nicht erfolgreiche Antwort liefern;
- Timeout kontrolliert behandeln;
- HTTP-Status nicht mit Transportfehler verwechseln;
- Responsegröße bereits während des Downloads begrenzen;
- keine unbeschränkte Bufferakkumulation zulassen;
- keine Exceptions aus der öffentlichen Methode lassen;
- pro Request unabhängig sein;
- unter TSan sauber sein.

### 12.5 `LocalFileRepositoryTransport`

MUSS:

- lokale Repositoryroots unterstützen;
- Binärmodus verwenden;
- Root-Containment prüfen;
- Symlink-/Canonicalization-Regeln dokumentieren;
- kein `..`-Escape erlauben;
- keine Remote-URI akzeptieren;
- fehlende optionale `ilisite.xml` korrekt klassifizieren.

### 12.6 `FetchRepositoryTransport`

MUSS:

- in Browser und moderner Node-Laufzeit funktionieren;
- `AbortSignal` durchreichen;
- Responsegröße begrenzen;
- nicht automatisch `response.text()` verwenden, bevor die Größe geprüft ist;
- `Uint8Array` liefern;
- CORS-Fehler verständlich melden;
- Redirectergebnis prüfen;
- keine Cachepolicy selbst implementieren.

### 12.7 Legacy-`load`-Option

Die bestehende Option:

```ts
load?: (uri: string) => Promise<Uint8Array | string>;
```

MUSS in P4 zunächst weiter funktionieren.

Sie wird über einen Adapter umgesetzt:

```ts
class LegacyLoadTransport implements RepositoryTransport {
  constructor(load);
  get(request);
}
```

Neue interne Logik DARF nicht direkt `options.load` aufrufen.

---

## 13. Port: Cache

### 13.1 Normalisierter C++-Vertrag

```cpp
struct RepositoryCacheEntry {
    std::string value;
    std::chrono::system_clock::time_point storedAt;
};

struct RepositoryCacheLookup {
    bool hit = false;
    RepositoryCacheEntry entry;
    std::string error;
};

struct RepositoryCacheStoreResult {
    bool success = false;
    std::string error;
    std::filesystem::path localPath;
};

class RepositoryCachePort {
public:
    virtual ~RepositoryCachePort() = default;

    virtual RepositoryCacheLookup get(std::string_view key) = 0;

    virtual RepositoryCacheStoreResult put(
        std::string_view key,
        std::string_view value,
        std::chrono::system_clock::time_point storedAt
    ) = 0;

    virtual void remove(std::string_view key) = 0;
    virtual void clear() = 0;
};
```

### 13.2 JavaScript-Vertrag

```ts
export interface CacheEntry {
  value: Uint8Array;
  storedAt: number;
}

export interface RepositoryCache {
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  delete?(key: string): Promise<void>;
  clear(): Promise<void>;
}
```

Der bestehende JavaScript-Vertrag bleibt kompatibel. Intern wird er durch einen Adapter normalisiert.

Neue Cacheimplementierungen SOLLEN zusätzlich unterstützen:

```ts
putEntry?(key: string, entry: CacheEntry): Promise<void>;
```

Wenn `putEntry` fehlt, darf der Adapter `put` verwenden. Der Cache erhält dann die injizierte Uhr über seinen Konstruktor.

### 13.3 Frischeklassifikation

```cpp
enum class CacheFreshness {
    Missing,
    Fresh,
    Stale
};
```

Die Klassifikation erfolgt ausschließlich durch:

```cpp
CacheFreshness classifyCacheEntry(
    const RepositoryCacheEntry& entry,
    time_point now,
    milliseconds ttl
);
```

Randfälle:

- `now == storedAt + ttl` ist stale;
- `storedAt > now` erzeugt eine Warnung und wird konservativ als fresh für höchstens TTL oder als stale behandelt; die Entscheidung muss in beiden Laufzeiten identisch sein;
- TTL null bedeutet sofort stale;
- Integerüberlauf darf nicht vorkommen;
- negative TTL wird vorher abgelehnt.

### 13.4 Cachefehler

Ein Cachefehler ist grundsätzlich nicht fatal, wenn Transportdaten erfolgreich verfügbar sind.

Beispiele:

- `put` schlägt fehl: Warnung, Source bleibt nutzbar;
- `get` schlägt fehl: Warnung, wie Cache Miss weiterfahren;
- `delete` schlägt fehl: Warnung, Bypass-Download trotzdem versuchen;
- `clear` schlägt fehl: expliziter API-Aufruf darf Fehler melden.

### 13.5 Stale-on-error

Stale-Fallback ist nur zulässig bei:

- Netzwerkfehler;
- Timeout;
- DNS-Fehler;
- retryable 5xx;
- vergleichbarer vorübergehender Transportstörung.

Stale-Fallback ist nicht zulässig bei:

- Prüfsummenabweichung;
- unsicherem Pfad;
- ungültigem XML;
- Dependency Cycle;
- fachlich nicht passendem Schema;
- expliziter Cancellation;
- Überschreitung eines Sicherheitslimits;
- erfolgreichem 404 für ein Pflichtartefakt;
- Cacheinhalt, der nicht gelesen oder decodiert werden kann.

### 13.6 Offline

Im Offline-Modus:

- wird kein Transport aufgerufen;
- ein frischer Cacheeintrag wird verwendet;
- ein stale Cacheeintrag wird verwendet und als stale markiert;
- ein fehlender Pflichtcacheeintrag erzeugt `ILIC-REPO-OFFLINE`;
- ein fehlender optionaler Site-Eintrag darf ohne Errorresultat behandelt werden;
- lokale Repositorypfade bleiben lesbar, sofern die Policy dies erlaubt;
- Prüfsummen werden weiterhin geprüft.

### 13.7 Atomare Writes

`NodeFileCache` und native `FileRepositoryCache` MÜSSEN:

- Content zuerst vollständig schreiben;
- atomar publizieren;
- Metadaten nie mit fremdem Content paaren;
- konkurrierende Writer tolerieren;
- Windows-Rename-Semantik berücksichtigen;
- temporäre Dateien bereinigen;
- Crashreste als Cache Miss behandeln;
- alte Generationen kontrolliert bereinigen;
- niemals halb geschriebene Daten als Hit liefern.

### 13.8 Browsercache

`BrowserCache` MUSS:

- eine versionierte IndexedDB-Struktur verwenden;
- Upgrades idempotent durchführen;
- Transaktionen korrekt abschließen;
- Datenbankhandles auch bei Fehlern schließen;
- `Uint8Array` stabil rekonstruieren;
- mehrere Tabs tolerieren;
- Quota-Fehler als Cachewarnung behandeln;
- eine Migration des bisherigen Stores `ilic-repository-v1/resources` unterstützen oder dessen Daten kontrolliert als alte Version weiterverwenden.

---

## 14. Port: Metadaten-Decoder

### 14.1 C++-Schnittstelle

```cpp
struct RepositoryIndex {
    std::string repository;
    std::vector<ModelMetadata> models;
};

struct RepositorySite {
    std::vector<std::string> parentSites;
    std::vector<std::string> subsidiarySites;
};

struct DecodeResultBase {
    bool success = false;
    std::vector<Diagnostic> diagnostics;
};

struct ModelIndexDecodeResult : DecodeResultBase {
    RepositoryIndex index;
};

struct SiteDecodeResult : DecodeResultBase {
    RepositorySite site;
};

class RepositoryMetadataDecoder {
public:
    virtual ~RepositoryMetadataDecoder() = default;

    virtual ModelIndexDecodeResult decodeModelIndex(
        std::string_view bytes,
        std::string_view repository
    ) = 0;

    virtual SiteDecodeResult decodeSite(
        std::string_view bytes,
        std::string_view repository
    ) = 0;
};
```

### 14.2 Regeln

Der Decoder MUSS:

- XML Namespaces tolerant behandeln;
- bekannte Elementnamen exakt auswerten;
- Entities korrekt decodieren;
- UTF-8 korrekt behandeln;
- externe Entities deaktivieren;
- DTD-Verarbeitung deaktivieren;
- Netzwerkzugriff während XML-Parsing verbieten;
- maximale XML-Größe beachten;
- maximale Verschachtelung begrenzen;
- ungültige Einträge diagnostizieren;
- gültige Geschwistereinträge trotz einzelner ungültiger Einträge lesen;
- Dokumentreihenfolge erhalten;
- keine regulären Ausdrücke als XML-Parser verwenden.

### 14.3 JavaScript

Die heutige Regex-basierte XML-Auswertung MUSS entfernt werden.

Die neue JavaScript-Implementierung MUSS einen standardskonformen Parser verwenden, der in Node und Browser funktioniert, beispielsweise:

- ein kleiner SAX-Parser;
- ein auditierter, plattformneutraler XML-Parser;
- eine gemeinsame Library, die in beiden Adaptern identisch eingesetzt wird.

Browser-`DOMParser` und ein völlig anderer Node-Parser sind nur zulässig, wenn gemeinsame Contract-Fixtures exakte Parität belegen. Bevorzugt wird ein gemeinsamer Decoder.

### 14.4 Fehlergrade

- ein kaputtes Repositoryindex-Dokument ist recoverable, solange weitere Repositories verfügbar sind;
- ein komplett leeres oder unbrauchbares Indexdokument macht dieses Repository unavailable;
- `ilisite.xml` ist optional;
- ein syntaktisch kaputtes vorhandenes `ilisite.xml` erzeugt eine Warnung;
- fehlendes `ilisite.xml` erzeugt keine normale Benutzerwarnung;
- XML-Sicherheitsverletzungen erzeugen Fehler und dürfen keinen Stale-Fallback auf denselben fehlerhaften Inhalt auslösen.

---

## 15. Port: Prüfsumme

### 15.1 Schnittstellen

C++:

```cpp
class RepositoryChecksum {
public:
    virtual ~RepositoryChecksum() = default;
    virtual std::string md5(std::string_view bytes) = 0;
};
```

JavaScript:

```ts
export interface RepositoryChecksum {
  md5(bytes: Uint8Array): string | Promise<string>;
}
```

### 15.2 Regeln

- MD5 bleibt unterstützt, weil das INTERLIS-Repositoryformat es verwendet;
- MD5 ist hier eine Kompatibilitäts-/Integritätsprüfung und kein Vertrauensanker;
- Vergleiche sind case-insensitive;
- Whitespace wird nicht entfernt;
- geprüft werden exakt die geladenen Bytes;
- bei Cache-Mismatch wird genau ein Bypass-Download versucht;
- bei erneutem Mismatch wird kein Stale-Inhalt verwendet;
- die Fehlermeldung enthält expected und actual;
- die Source wird nicht an den Compiler übergeben.

### 15.3 JavaScript-MD5

Die heutige MD5-Implementierung darf nicht mehrfach kopiert werden.

Zulässig:

- ein einzelnes plattformneutrales MD5-Modul;
- Node-Adapter über `node:crypto`;
- Browseradapter über einen einzigen getesteten Fallback.

Alle Adapter müssen dieselben Testvektoren bestehen.

---

## 16. `RepositoryResourceCoordinator`

### 16.1 Verantwortung

Der Coordinator ist die einzige Komponente, die entscheidet:

- Cache zuerst oder Transport;
- fresh oder stale;
- Offline;
- Stale-on-error;
- Cache-Write;
- Bypass-Retry bei Prüfsummenfehler;
- Provenienzflags;
- Warnungen.

### 16.2 C++-Schnittstelle

```cpp
struct RepositoryResourceRequest {
    RepositoryUri uri;
    RepositoryResourceKind kind;
    std::chrono::milliseconds ttl;
    bool optional = false;
    std::string expectedMd5;
};

struct RepositoryResource {
    bool success = false;
    bool fromCache = false;
    bool stale = false;
    std::string uri;
    std::string content;
    std::filesystem::path localPath;
    std::string error;
    std::vector<Diagnostic> diagnostics;
};

class RepositoryResourceCoordinator final {
public:
    RepositoryResourceCoordinator(
        const RepositoryPolicy& policy,
        RepositoryTransport& transport,
        RepositoryCachePort& cache,
        RepositoryClock& clock,
        RepositoryChecksum& checksum
    );

    RepositoryResource load(
        const RepositoryResourceRequest& request
    );
};
```

### 16.3 Entscheidungsalgorithmus

Für jeden Request:

1. URI validieren.
2. Cachekey aus normalisierter URI erzeugen.
3. Cache lesen.
4. Cachefehler als Warnung speichern.
5. Frische klassifizieren.
6. Bei fresh: Cache liefern.
7. Bei offline und hit: Cache liefern, gegebenenfalls stale.
8. Bei offline und miss:
   - optional: unavailable ohne Error;
   - Pflicht: `ILIC-REPO-OFFLINE`.
9. Transport ausführen.
10. Erfolgreiche Bytes auf Größenlimit prüfen.
11. Bei Transportfehler:
    - optionales 404: unavailable ohne Error;
    - stale hit und erlaubter transienter Fehler: stale liefern;
    - sonst Fehler.
12. Bei Erfolg Cache schreiben.
13. Cache-Write-Fehler als Warnung.
14. Bei erwarteter MD5:
    - prüfen;
    - falls Mismatch aus Cache und online: Cache invalidieren und exakt einmal Bypass laden;
    - erneut prüfen;
    - Mismatch fatal.
15. Resultat mit `fromCache`, `stale`, URI und Source zurückgeben.

### 16.4 Nichtverantwortungen

Der Coordinator:

- traversiert keine Sites;
- wählt keine Version;
- löst keine Dependencies;
- parst kein XML;
- installiert nichts im Compiler;
- erzeugt keine CLI-Ausgabe.

---

## 17. `RepositoryCatalog`

### 17.1 Verantwortung

Der Catalog verwaltet pro `RepositoryManager`:

- geladene Indizes;
- geladene Site-Metadaten;
- negative Loads;
- Load-Diagnosen;
- optional Single-Flight-Zustand.

Er besitzt keine globale State.

### 17.2 C++-Schnittstelle

```cpp
class RepositoryCatalog final {
public:
    RepositoryCatalog(
        RepositoryResourceCoordinator& resources,
        RepositoryMetadataDecoder& decoder
    );

    const RepositoryIndex* index(
        const RepositoryUri& repository,
        std::vector<Diagnostic>& diagnostics
    );

    const RepositorySite* site(
        const RepositoryUri& repository,
        std::vector<Diagnostic>& diagnostics
    );

    void invalidate(const RepositoryUri& repository);
    void clearMemoryState();
};
```

### 17.3 Load States

```cpp
enum class CatalogLoadStatus {
    NotLoaded,
    Loading,
    Available,
    Unavailable
};
```

Ein negative Load darf innerhalb derselben Managerinstanz gecacht werden, aber:

- `invalidate` muss einen neuen Versuch erlauben;
- Offline-/Online-Umschaltung darf nicht versehentlich permanent blockiert werden;
- eine Cancellation darf keinen permanenten `Unavailable`-Eintrag erzeugen;
- transienter Fehler darf gemäß Policy neu versuchbar bleiben;
- Tests müssen dieses Verhalten festlegen.

### 17.4 Thread-/Concurrency-Vertrag

Native:

- parallele Calls auf derselben Managerinstanz müssen sicher sein oder explizit durch einen Manager-Mutex serialisiert werden;
- es darf kein Prozessglobal-Lock entstehen;
- derselbe Index soll nach Möglichkeit nur einmal gleichzeitig geladen werden;
- unterschiedliche Managerinstanzen müssen parallel laufen.

JavaScript:

- parallele Promises für dieselbe URI sollen über Single Flight koalesziert werden;
- ein abgelehntes Promise muss aus der In-Flight-Map entfernt werden;
- Cancellation eines Callers darf andere Caller nicht zwangsläufig abbrechen.

---

## 18. `RepositoryGraphWalker`

### 18.1 Suchreihenfolge

Die Suchreihenfolge muss die bestehende Semantik erhalten:

1. konfigurierte Seed-Repositories in Konfigurationsreihenfolge;
2. Parent Sites;
3. Subsidiary Sites;
4. Parent Sites, die von Subsidiary Sites entdeckt werden, vor weiteren tiefen Subsidiaries entsprechend dem bestehenden Algorithmus.

Der Agent muss die heutige native und JavaScript-Reihenfolge mit Fixtures erfassen, bevor er refactort.

### 18.2 Schnittstelle

```cpp
struct RepositoryVisit {
    RepositoryUri repository;
    enum class Relation {
        Seed,
        Parent,
        Subsidiary
    } relation;
    std::size_t depth = 0;
};

class RepositoryGraphWalker final {
public:
    explicit RepositoryGraphWalker(
        RepositoryCatalog& catalog,
        const RepositoryPolicy& policy
    );

    std::vector<RepositoryVisit> traversalOrder(
        const std::vector<RepositoryUri>& seeds,
        std::vector<Diagnostic>& diagnostics
    );

    ModelLookupResult findModel(
        const std::vector<RepositoryUri>& seeds,
        std::string_view modelName,
        std::string_view schemaLanguage
    );

    RepositoryListResult listModels(
        const std::vector<RepositoryUri>& seeds
    );
};
```

### 18.3 Invarianten

- jede normalisierte Repository-URI höchstens einmal besuchen;
- Zyklen in `ilisite.xml` terminieren;
- Seed-Reihenfolge erhalten;
- keine Hash-Reihenfolge exponieren;
- Sicherheitslimit für besuchte Sites;
- invalides Linkziel als Diagnose;
- relative Site-Links gegen aktuelle Repository-URI auflösen;
- Linktypen nicht vermischen;
- `followSiteLinks=false` besucht nur Seeds.

---

## 19. `ModelVersionSelector`

### 19.1 Schnittstelle

```cpp
class ModelVersionSelector final {
public:
    VersionSelectionResult select(
        const std::vector<ModelMetadata>& candidates,
        std::string_view modelName,
        std::string_view schemaLanguage
    ) const;
};
```

### 19.2 Regeln

Die bestehende Präferenz muss erhalten bleiben:

```text
ili2_4
ili2_3
ili1
```

wenn keine explizite Sprache verlangt wird.

Bei expliziter Sprache:

- nur exakt passende Sprache;
- keine stille Fallback-Sprache;
- `browseOnly=true` nicht als Kompilationskandidat, sofern bisher so definiert;
- Version/Publishing Date/Precursor-Regeln exakt aus dem vorhandenen Algorithmus übernehmen;
- uneindeutige Metadaten diagnostizieren;
- deterministische Auswahl;
- dieselbe Eingabeliste führt in Native und JavaScript zur selben Auswahl.

### 19.3 Testfälle

Mindestens:

- nur eine Version;
- mehrere Versionen;
- leere Version;
- Publishing Date;
- Precursor-Kette;
- gebrochene Precursor-Kette;
- Zyklus;
- doppelte identische Einträge;
- unterschiedliche Repositories;
- browseOnly;
- explizites `ili1`;
- explizites `ili2_3`;
- explizites `ili2_4`;
- unbekannte Sprache;
- gleiche Version mit abweichender Datei.

---

## 20. `RepositoryDependencyResolver`

### 20.1 Verantwortung

Der Resolver:

- löst Wurzelmodelle;
- ruft Model Lookup auf;
- traversiert Dependencies;
- erkennt Zyklen;
- dedupliziert Modelle;
- dedupliziert Dateien;
- lädt Modellbytes;
- erzeugt `ResolvedWorkspace`.

### 20.2 Schnittstelle

```cpp
class RepositoryDependencyResolver final {
public:
    RepositoryDependencyResolver(
        RepositoryGraphWalker& graph,
        RepositoryResourceCoordinator& resources,
        RepositoryChecksum& checksum,
        const RepositoryPolicy& policy
    );

    RepositoryResult resolve(
        const RepositoryResolutionRequest& request
    );
};
```

### 20.3 DFS-Zustand

```cpp
struct ResolutionState {
    std::vector<std::string> stack;
    std::set<std::string> resolvedModels;
    std::map<std::string, std::string> emittedFileChecksums;
    std::vector<ResolvedModel> output;
};
```

Es SOLLEN geordnete Container verwendet werden, wenn Reihenfolge sichtbar ist.

### 20.4 Zyklusdiagnose

Beispiel:

```text
A -> B -> C -> A
```

Diagnose:

```text
code: ILIC-REPO-CYCLE
severity: error
message: dependency cycle: A -> B -> C -> A
```

Die Zykluskette muss:

- bei der ersten Wiederholung beginnen;
- keine Pointer oder internen IDs enthalten;
- deterministisch sein;
- Native/JavaScript-identisch sein.

### 20.5 Mehrere Modelle in einer Datei

Wenn mehrere Modellmetadaten auf dieselbe URI zeigen:

- Datei nur einmal laden;
- Prüfsumme jedes Metadateneintrags gegen denselben tatsächlichen MD5 prüfen;
- widersprüchliche erwartete MD5 als Fehler;
- Source nur einmal im Workspace ausgeben;
- alle Modellnamen als resolved markieren, sobald ihre eigenen Dependencies aufgelöst sind;
- keine doppelte Compilerinstallation.

### 20.6 `INTERLIS`

Das eingebaute Modell `INTERLIS` wird nicht aus Repositories geladen.

Der Resolver muss es als erfüllt behandeln.

---

## 21. Repository-Diagnosen

### 21.1 Stabilität

Bestehende Diagnosecodes müssen soweit möglich erhalten bleiben:

- `ILIC-REPO-INDEX`
- `ILIC-REPO-CACHE`
- `ILIC-REPO-CYCLE`
- `ILIC-REPO-NOT-FOUND`
- `ILIC-REPO-PATH`
- `ILIC-REPO-CHECKSUM`
- `ILIC-REPO-DOWNLOAD`

Additive Codes:

- `ILIC-REPO-CONFIG`
- `ILIC-REPO-URI`
- `ILIC-REPO-SITE`
- `ILIC-REPO-XML`
- `ILIC-REPO-VERSION`
- `ILIC-REPO-OFFLINE`
- `ILIC-REPO-LIMIT`
- `ILIC-REPO-CANCELLED`
- `ILIC-REPO-INSTALL`

### 21.2 Struktur

JavaScript muss eine strukturierte Form verwenden:

```ts
export interface RepositoryDiagnostic {
  severity: "error" | "warning" | "information";
  code: string;
  message: string;
  uri?: string;
  operation?:
    | "config"
    | "metadata"
    | "site"
    | "version"
    | "model"
    | "cache"
    | "transport"
    | "checksum"
    | "install";
}
```

### 21.3 Kompatibilität von `onWarning`

Bestehende Consumer mit:

```ts
onWarning?: (warning) => void
```

müssen weiter funktionieren.

Intern werden Diagnostics erzeugt. Der Compatibility Layer ruft `onWarning` nur für Warning-Diagnostics auf.

### 21.4 Fehlerobjekt

Additiv SOLL JavaScript erhalten:

```ts
export class RepositoryError extends Error {
  readonly diagnostics: RepositoryDiagnostic[];
  readonly partialWorkspace?: ResolvedWorkspace;
}
```

Die bestehenden `resolveWorkspace`-Methoden dürfen weiterhin rejecten.

Zusätzlich KANN eingeführt werden:

```ts
resolveWorkspaceResult(
  request: RepositoryResolutionRequest
): Promise<RepositoryResult>;
```

Dabei muss die alte API erhalten bleiben.

---

## 22. C++-Fassade `RepositoryManager`

### 22.1 Öffentliche Kompatibilität

Folgende APIs bleiben:

```cpp
RepositoryManager(RepositoryOptions options = {});
const RepositoryOptions& options() const;
std::vector<ModelMetadata> listModels();
RepositoryResult resolve(
    const std::vector<std::string>& models,
    const std::string& schemaLanguage
);
RepositoryResult resolve(
    const std::string& model,
    const std::string& schemaLanguage
);
static std::vector<std::string> defaultRepositories();
static std::vector<ModelMetadata> parseIliModelsXml(...);
```

### 22.2 Konstruktion mit Ports

Für Tests und Einbettung SOLL additiv möglich sein:

```cpp
struct RepositoryPorts {
    std::unique_ptr<RepositoryTransport> transport;
    std::unique_ptr<RepositoryCachePort> cache;
    std::unique_ptr<RepositoryClock> clock;
    std::unique_ptr<RepositoryMetadataDecoder> metadataDecoder;
    std::unique_ptr<RepositoryChecksum> checksum;
};

RepositoryManager(
    RepositoryOptions options,
    RepositoryPorts ports
);
```

Alternativ kann eine Factory verwendet werden.

### 22.3 Ownership

`RepositoryManager` besitzt seine Ports standardmäßig.

Eine Testfactory darf Borrowing ermöglichen, wenn Lebensdauer eindeutig ist. Bevorzugt wird Ownership.

`Impl`:

- nicht global;
- nicht kopierbar;
- bewegbar wie bisher;
- thread-safe gemäß dokumentiertem Vertrag;
- löscht Ressourcen deterministisch.

---

## 23. JavaScript-Core und stabile `@ilic/tools`-Fassade

### 23.1 Neues Low-Level-Core-Paket

Bevorzugtes Ziel:

```text
packages/repository-core
package name: @ilic/repository-core
```

Das Paket enthält:

- keine Node-Imports;
- keine Browserglobals;
- keinen impliziten Fetch;
- kein IndexedDB;
- keine Dateisystemzugriffe;
- keine direkte Systemzeit;
- keine Compiler-WASM-Abhängigkeit;
- keine Consumer-Kompatibilitätsheuristik;
- keine automatische Wahl eines Laufzeitadapters.

Die öffentliche Hauptklasse des Core-Pakets MUSS einen vom bisherigen Namen unterscheidbaren Low-Level-Namen tragen:

```ts
export class RepositoryManagerCore {
  constructor(options: RepositoryManagerCoreOptions);

  listModels(): Promise<ModelMetadata[]>;
  resolveWorkspace(
    models: string[],
    schemaLanguage?: string
  ): Promise<ResolvedWorkspace>;
  resolveModel(
    model: string,
    schemaLanguage?: string
  ): Promise<ResolvedWorkspace>;
}
```

Alternativ ist ein gleich klarer Name wie `RepositoryResolverCore` zulässig. Der Core DARF NICHT selbst `RepositoryManager` exportieren, wenn dies zu einer versehentlichen Ersetzung der bestehenden High-Level-API führt.

Die Core-Optionen verlangen explizite Ports:

```ts
export interface RepositoryManagerCoreOptions {
  repositories: string[];
  transport: RepositoryTransport;
  cache: RepositoryCache;
  clock: RepositoryClock;
  metadataDecoder: RepositoryMetadataDecoder;
  checksum: RepositoryChecksum;
  policy?: Partial<RepositoryPolicy>;
  observer?: RepositoryObserver;
  signal?: AbortSignal;
}
```

### 23.2 `@ilic/tools` bleibt die öffentliche Compatibility Facade

`@ilic/tools` MUSS weiterhin selbst folgende Namen exportieren:

```ts
export class RepositoryManager { /* compatibility facade */ }
export class MemoryCache implements RepositoryCache { /* compatibility facade */ }

export {
  parseIliModelsXml,
  parseIliSiteXml,
  selectLatestModelVersion,
  supportedSchemaLanguagePreference,
  normalizeRepositoryUri,
  resolveRepositoryUri,
  validateRepositoryRelativePath
};
```

Bestehende Imports bleiben ohne Änderung gültig:

```ts
import {
  RepositoryManager,
  MemoryCache,
  parseIliModelsXml,
  parseIliSiteXml
} from "@ilic/tools";

import { NodeFileCache } from "@ilic/tools/node";
import { BrowserCache } from "@ilic/tools/browser";
```

Die Fassade darf reine Funktionen direkt aus `@ilic/repository-core` re-exportieren. Sie DARF jedoch die Klassen `RepositoryManager` und `MemoryCache` nicht bloß re-exportieren, weil deren bestehende High-Level-Defaults im Core absichtlich nicht vorhanden sind.

Ausdrücklich verboten ist daher:

```js
// VERBOTEN: bricht den bestehenden Default- und Legacyvertrag.
export {
  RepositoryManager,
  MemoryCache
} from "@ilic/repository-core";
```

### 23.3 Verantwortung der öffentlichen `RepositoryManager`-Fassade

Die Fassade MUSS:

1. sämtliche bisherigen Konstruktoroptionen akzeptieren;
2. Legacyoptionen in Core-Ports und `RepositoryPolicy` übersetzen;
3. einen explizit übergebenen `transport` unverändert verwenden;
4. die bestehende `load`-Option über einen `LoadFunctionRepositoryTransport` adaptieren;
5. ohne `transport` und ohne `load` den bestehenden HTTP-/HTTPS-Komfortpfad über `globalThis.fetch` bereitstellen;
6. einen `MemoryCache` als bisherigen Defaultcache verwenden, wenn kein Cache angegeben wurde;
7. eine konkrete Systemuhr ausschließlich im Facade-/Adapterlayer erzeugen;
8. `onWarning` aus strukturierten Diagnostics bedienen;
9. exakt die bisherigen High-Level-Methoden anbieten;
10. Ergebnis- und Fehlerformen kompatibel halten.

Sinngemäße interne Struktur:

```ts
export class RepositoryManager {
  readonly #core: RepositoryManagerCore;

  constructor(options: RepositoryManagerOptions = {}) {
    const clock = options.clock ?? new SystemRepositoryClock();
    const cache = options.cache ?? new MemoryCache({ clock });
    const transport = selectCompatibilityTransport(options);

    this.#core = new RepositoryManagerCore({
      repositories: normalizeConfiguredRepositories(options.repositories),
      transport,
      cache,
      clock,
      metadataDecoder:
        options.metadataDecoder ?? new SafeRepositoryMetadataDecoder(),
      checksum: options.checksum ?? new Md5RepositoryChecksum(),
      policy: translateLegacyOptions(options),
      observer: compatibilityObserver(options.onWarning),
      signal: options.signal
    });
  }

  listModels() {
    return this.#core.listModels();
  }

  resolveWorkspace(models: string[], schemaLanguage = "") {
    return this.#core.resolveWorkspace(models, schemaLanguage);
  }

  resolveModel(model: string, schemaLanguage = "") {
    return this.#core.resolveModel(model, schemaLanguage);
  }
}
```

Dies ist Verantwortungsbeispiel, keine Verpflichtung zu exakt dieser privaten Feldstruktur.

### 23.4 Bestehender Konstruktorvertrag

Folgende Form MUSS weiter kompilieren und laufen:

```ts
new RepositoryManager({
  repositories,
  cache,
  load,
  offline,
  metadataTtlMs,
  modelTtlMs,
  allowStaleOnError,
  followSiteLinks,
  validateChecksums,
  onWarning
});
```

Additiv zulässig:

```ts
new RepositoryManager({
  transport,
  clock,
  checksum,
  metadataDecoder,
  limits,
  signal,
  observer
});
```

Prioritätsreihenfolge:

1. explizites `transport`;
2. aus `load` erzeugter Compatibility-Transport;
3. expliziter Default-Fetch-Adapter der `@ilic/tools`-Fassade.

Wenn `transport` und `load` gleichzeitig gesetzt sind:

- `transport` gewinnt;
- genau eine strukturierte Konfigurationswarnung wird erzeugt;
- `load` wird nicht aufgerufen;
- das Verhalten wird getestet und dokumentiert.

### 23.5 Default-Fetch ist Facadeverhalten, nicht Coreverhalten

Der Core DARF `globalThis.fetch` nicht lesen.

Die Fassade DARF für die rückwärtskompatible Konstruktion einen Fetch-Adapter erzeugen, sofern `globalThis.fetch` eine Funktion ist. Dies ist in modernen Browsern und den vom Projekt unterstützten Node-Versionen der bestehende Komfortpfad.

Falls weder `transport`, noch `load`, noch `globalThis.fetch` vorhanden ist, MUSS die Fassade kontrolliert und verständlich fehlschlagen:

```text
No repository transport is available. Provide `load`, `transport`,
or use `createNodeRepositoryManager` / `createBrowserRepositoryManager`.
```

Kein `ReferenceError`, kein stiller Offline-Modus und kein Import eines Node-Moduls im Browserbundle.

### 23.6 `MemoryCache`-Kompatibilität

Der bestehende Aufruf bleibt gültig:

```ts
const cache = new MemoryCache();
```

Der Core darf intern einen expliziten `MemoryRepositoryCache` verlangen, der eine Uhr benötigt. Die öffentliche `MemoryCache`-Fassade MUSS dann eine Systemuhr injizieren und den alten Konstruktor erhalten.

Additiv zulässig:

```ts
new MemoryCache({
  clock,
  cloneBytes: true,
  faultInjection
});
```

`get`, `put`, `delete` und `clear` behalten Signaturen und Promise-Verhalten.

### 23.7 Öffentliche Ergebnistypen bleiben stabil

Folgende Form bleibt normativ:

```ts
export interface ResolvedWorkspace {
  models: Array<{
    metadata: ModelMetadata;
    uri: string;
    source: string;
    fromCache: boolean;
    stale: boolean;
  }>;
}
```

P4 DARF bestehende Felder weder entfernen noch umbenennen noch in ihrer Bedeutung ändern.

Additive Provenienzfelder sind nur optional zulässig, beispielsweise:

```ts
localPath?: string;
cacheKey?: string;
resolvedFrom?: string;
```

Für Browser und universelle Consumer darf `localPath` nicht vorausgesetzt werden. `source` bleibt immer die autoritative Modellquelle.

### 23.8 Methoden- und Fehlerkompatibilität

Diese Methoden bleiben:

```ts
listModels(): Promise<ModelMetadata[]>;
resolveWorkspace(
  models: string[],
  schemaLanguage?: string
): Promise<ResolvedWorkspace>;
resolveModel(
  model: string,
  schemaLanguage?: string
): Promise<ResolvedWorkspace>;
```

Die bestehende Promise-basierte Fehlerbehandlung bleibt. Ein additiver `RepositoryError` darf reichere Diagnostics tragen, muss aber weiterhin `instanceof Error` sein und eine verständliche `message` besitzen.

Ein bestehender Consumer, der nur `error.message` liest, muss weiter funktionieren.

### 23.9 Keine Produktionscode-Migration für Node-LSP und Web-IDE

Node-LSP und Web-IDE müssen weiterhin verwenden können:

```ts
const manager = new RepositoryManager(options);
const workspace = await manager.resolveWorkspace(models, schemaLanguage);
session.putWorkspace(workspace);
```

Nicht erforderlich sein dürfen:

- Import von `RepositoryManagerCore`;
- Import aus `@ilic/repository-core`;
- rohe C-ABI-Aufrufe;
- eine andere Workspaceform;
- ein manueller Loop über `putSource`;
- eine neue Compilerklasse;
- Netzwerkzugriff im WASM-Kern;
- ein asynchrones `editorSnapshot()`;
- Zugriff auf `localPath`;
- manuelle Wahl eines Browser- oder Node-Transports für den bisherigen HTTP-Standardfall.

Neue Anwendungen DÜRFEN explizite Factories verwenden. Das ist eine Empfehlung, keine Migration für bestehende Anwendungen.

### 23.10 Private Deep Imports

Nur dokumentierte Package-Exports sind kompatibilitätsgeschützt.

Consumer, die private Dateien wie:

```ts
import "@ilic/tools/repository-manager.js";
```

verwenden, dürfen auf öffentliche Exporte migriert werden. Der Agent MUSS vor einer solchen Änderung jedoch nachweisen, dass der Consumer tatsächlich einen privaten Deep Import verwendet.

P4 DARF keinen hypothetischen Deep-Import-Bruch als Begründung verwenden, um die offizielle API zu ändern.

### 23.11 TypeScript-API-Diff

Vor und nach P4 MUSS ein maschinenlesbarer Public-API-Diff erstellt werden.

Mindestens geprüft werden:

- Exportnamen;
- Subpath-Exports;
- Klassenkonstruktoren;
- Optionsfelder;
- Methodennamen;
- Parameterreihenfolge;
- Promise-Rückgabetypen;
- `ResolvedWorkspace`;
- `ModelMetadata`;
- Cacheinterfaces;
- Warningcallback.

Entfernte oder inkompatibel geänderte öffentliche Elemente sind P4-Fehler, außer die Spezifikation erlaubt die Änderung ausdrücklich.

## 24. Node-Adapter

### 24.1 `NodeFileCache`

Die bestehende Klasse bleibt.

Sie MUSS zusätzlich:

- eine injizierbare Uhr unterstützen;
- Cachelayout versionieren;
- aktuelle Daten migrieren oder weiter lesen;
- defekte Metadaten als Miss behandeln;
- Contentintegrität prüfen;
- Concurrent Writer testen;
- Generationen bereinigen;
- sichere Dateinamen verwenden;
- Cachekey nicht als Pfad verwenden;
- Windows, macOS und Linux unterstützen.

### 24.2 `NodeRepositoryTransport`

```ts
export class NodeRepositoryTransport
  implements RepositoryTransport {
  constructor(options?: {
    fetch?: typeof globalThis.fetch;
    fileRoots?: string[];
    maxRedirects?: number;
  });

  get(request: RepositoryTransportRequest):
    Promise<RepositoryTransportResponse>;
}
```

Er unterstützt:

- HTTP;
- HTTPS;
- `file:` und konfigurierte lokale Pfade;
- Byte-Limits;
- Cancellation;
- Root-Containment;
- keine implizite Shell;
- keine Umgebungsvariable im Log.

### 24.3 Node-Factory

```ts
export function createNodeRepositoryManager(
  options?: RepositoryManagerOptions & {
    cacheDirectory?: string;
  }
): RepositoryManager;
```

Die Factory ist additiv. Bestehende Node-Consumer müssen sie für normale HTTP-/HTTPS-Repositories nicht verwenden. Sie ist die bevorzugte neue API, wenn lokale Pfade, `file:`-Repositories oder ein persistenter `NodeFileCache` benötigt werden.

### 24.4 Erhalt des Node-Subpath-Exports

`@ilic/tools/node` MUSS mindestens weiterhin `NodeFileCache` exportieren. Neue Exporte dürfen additiv ergänzt werden:

```ts
export {
  NodeFileCache,
  NodeRepositoryTransport,
  createNodeRepositoryManager
};
```

Das Node-Entry-Point darf nicht in Browserbundles gelangen, sofern der Consumer es nicht explizit importiert.

---

## 25. Browser-Adapter

### 25.1 `FetchRepositoryTransport`

Siehe Transportvertrag.

### 25.2 `BrowserCache`

Die bestehende Klasse bleibt und erhält:

```ts
constructor(
  database?: string,
  options?: {
    clock?: RepositoryClock;
    version?: number;
  }
)
```

### 25.3 Browser-Factory

```ts
export function createBrowserRepositoryManager(
  options?: RepositoryManagerOptions & {
    database?: string;
  }
): RepositoryManager;
```

Die Factory ist additiv. Die bestehende Konstruktion `new RepositoryManager({ repositories })` muss im Browser für HTTP-/HTTPS-Repositories weiterhin funktionieren. Die Factory ist die bevorzugte neue API, wenn `BrowserCache`, Cache-Datenbankname oder Fetch-Optionen explizit konfiguriert werden sollen.

### 25.4 Erhalt des Browser-Subpath-Exports

`@ilic/tools/browser` MUSS mindestens weiterhin `BrowserCache` exportieren. Neue Exporte dürfen additiv ergänzt werden:

```ts
export {
  BrowserCache,
  FetchRepositoryTransport,
  createBrowserRepositoryManager
};
```

### 25.5 Browser-Sicherheitsregeln

- CORS-Fehler werden als Transportdiagnose ausgegeben;
- keine Umgehung mit `no-cors`;
- keine Credentials standardmäßig;
- `credentials` nur explizit;
- Redirects auf nicht erlaubte Schemes ablehnen;
- `file:` im Browser ablehnen;
- Cachequota nicht als Compilerfehler behandeln;
- Service Worker darf Responses cachen, aber RepositoryCache bleibt fachlich autoritativ für TTL/Offline.

---

## 26. Memory- und Testadapter

### 26.1 `MemoryCache`

MUSS:

- Bytes kopieren oder Unveränderlichkeit garantieren;
- `storedAt` über injizierte Uhr setzen;
- `delete`;
- `clear`;
- optionale Fault Injection ermöglichen.

### 26.2 `FakeTransport`

```ts
export class FakeRepositoryTransport
  implements RepositoryTransport {
  add(uri: string, response: FakeResponse): void;
  fail(uri: string, error: string, retryable?: boolean): void;
  calls(): readonly RepositoryTransportRequest[];
  resetCalls(): void;
}
```

C++ erhält eine entsprechende Testklasse.

### 26.3 `ManualClock`

Muss Native und JavaScript dieselben Zeitfixtures unterstützen.

---

## 27. Direkte Compiler-Workspace-Integration

### 27.1 Neue Source-Origin

`CompilationSourceStore` MUSS erweitern:

```cpp
enum class SourceOrigin {
    Session,
    Repository,
    FileSystem,
    Builtin
};
```

### 27.2 Repositorysource

```cpp
const SourceBuffer& rememberRepositorySource(
    std::string uri,
    std::string utf8
);
```

Alternativ:

```cpp
const SourceBuffer& rememberSource(
    std::string uri,
    std::string utf8,
    SourceOrigin origin
);
```

Regeln:

- URI bleibt Repository-URI;
- Source wird im Context oder in der Session besessen;
- kein `string_view` auf temporären `ResolvedModel`;
- identische URI mit identischem Inhalt ist idempotent;
- identische URI mit anderem Inhalt erzeugt kontrollierte Ersetzung oder Fehler gemäß Sessionvertrag;
- Dokumentversion für Repositorysources ist deterministisch, beispielsweise 0;
- Repositorysource wird bei Compilerdiagnosen als URI angezeigt.

### 27.3 `RepositoryWorkspaceInstaller`

```cpp
struct RepositoryInstallResult {
    bool success = false;
    std::vector<std::string> installedUris;
    std::vector<Diagnostic> diagnostics;
};

class RepositoryWorkspaceInstaller final {
public:
    RepositoryWorkspaceInstaller(
        detail::CompilationSourceStore& sources,
        util::IliFileCatalog& files,
        util::Logger& logger
    );

    RepositoryInstallResult install(
        const RepositoryResult& resolved
    );
};
```

### 27.4 Installationsalgorithmus

Für jedes `ResolvedModel`:

1. `source` vorhanden und URI gültig prüfen.
2. Prüfen, ob URI bereits installiert ist.
3. Bei gleicher URI und identischer Source überspringen.
4. Bei gleicher URI und abweichender Source Fehler.
5. `rememberRepositorySource(uri, source)`.
6. `files.loadByFile(uri)`.
7. Prüfen, ob Metadatenmodell tatsächlich in der Sourceheaderanalyse vorkommt.
8. Datei selektieren.
9. installierte URI erfassen.

### 27.5 Keine lokale Dateivoraussetzung

Folgender Altpfad MUSS entfernt werden:

```cpp
files.loadByFile(model.localPath.string());
```

wenn er zur Installation eines Repositoryresultats dient.

Ziel:

```cpp
sources.rememberRepositorySource(model.uri, model.source);
files.loadByFile(model.uri);
```

### 27.6 Public Bridge

Additiv KANN bereitgestellt werden:

```cpp
RepositoryInstallResult putResolvedWorkspace(
    CompilerSession& session,
    const RepositoryResult& result
);
```

Der Compilerkern darf dafür nicht vom Repositorymodul abhängen. Die Funktion lebt im Bridge-Modul.

### 27.7 CompilerSession

`CompilerSession::putSource` bleibt unverändert.

Optional additiv:

```cpp
void putSources(
    const std::vector<SourceBuffer>& sources
);
```

Eine direkte Abhängigkeit auf `ResolvedWorkspace` ist im Compilerkern nicht zulässig.

### 27.8 JavaScript/WASM

Bestehend:

```ts
session.putWorkspace(workspace);
```

bleibt unverändert.

P4 darf den P3-Wrapper nicht wieder mit Repositoryfachlogik belasten.

---

## 28. Cachematerialisierung und `localPath`

### 28.1 Neue Semantik

`localPath` ist optionales Artefakt, keine Compilerquelle.

### 28.2 `TemporaryModelStore`

Der bisherige Zwang zur `TemporaryModelStore::materialize` nach erfolgreichem Download MUSS entfernt werden.

Eine Materialisierung darf nur erfolgen, wenn ein expliziter Consumer sie verlangt:

```cpp
MaterializationResult materialize(
    const ResolvedModel& model,
    const std::filesystem::path& destination
);
```

### 28.3 Read-only-Szenario

Folgender Ablauf MUSS funktionieren:

- Cacheverzeichnis read-only oder nicht vorhanden;
- Transport liefert gültiges Modell;
- Cache-Write erzeugt Warnung;
- Resolver liefert `source`;
- Compiler kompiliert erfolgreich aus Memory;
- `localPath` bleibt leer.

### 28.4 Cache Hit

Bei Disk-Cache-Hit darf `localPath` gesetzt sein.

Trotzdem muss der Compiler `source` verwenden.

---

## 29. URI- und Pfadsicherheit

### 29.1 Erlaubte Repositoryarten

Native:

- HTTPS;
- HTTP;
- `file:` URI;
- lokaler Pfad.

Browser:

- HTTPS;
- HTTP entsprechend Page/CORS;
- kein lokaler Pfad;
- kein `file:`.

Node:

- HTTPS;
- HTTP;
- `file:`;
- lokaler Pfad.

### 29.2 Relative Modellpfade

`validateRepositoryRelativePath` MUSS ablehnen:

- absolute POSIX-Pfade;
- Windows-Laufwerkspfade;
- UNC-Pfade;
- `..`;
- percent-encoded `..`;
- Backslash-Escape;
- NUL;
- URI-Scheme;
- Query/Fragment, sofern das Format dies nicht vorsieht;
- leere Pfade;
- Pfade, die nach Normalisierung Root verlassen.

### 29.3 Lokale Canonicalization

Native/Node:

- Repositoryroot canonical oder weakly canonical bestimmen;
- Zielpfad bestimmen;
- Symlinks berücksichtigen;
- Root-Containment nach Auflösung prüfen;
- TOCTOU-Risiko soweit praktikabel minimieren;
- Test für Symlink Escape.

### 29.4 Redirects

Ein Redirect:

- darf nicht unbeschränkt sein;
- darf nicht von HTTPS auf `file:` wechseln;
- darf nicht in lokale Pfade wechseln;
- final URI wird für Diagnose erfasst;
- Cachekey bleibt nach klar dokumentierter Regel ursprüngliche oder finale URI;
- diese Regel ist in Native/JS gleich.

Empfehlung: Cachekey bleibt normalisierte Request-URI; final URI wird Provenienz.

---

## 30. XML-Sicherheit

Mindesttests:

- externe Entity;
- DTD;
- Billion Laughs;
- tiefe Verschachtelung;
- sehr große Textnode;
- ungültiges UTF-8;
- Namespacepräfix;
- Defaultnamespace;
- unbekannte Elemente;
- CDATA;
- Entities;
- fehlende Pflichtfelder;
- doppelte Pflichtfelder;
- mehrere `ModelMetadata`-Versionen;
- kaputtes einzelnes Element bei gültigen Geschwistern.

Produktionscode DARF NICHT per Regex vollständiges XML parsen.

---

## 31. Determinismus

Folgende Ausgaben müssen deterministisch sein:

- normalisierte Repositoryliste;
- Traversierungsreihenfolge;
- Indexmodellreihenfolge;
- ausgewählte Version;
- Dependency-Reihenfolge;
- Workspace-Modellreihenfolge;
- Diagnose-Reihenfolge;
- Warnungsreihenfolge;
- Contract-JSON;
- Cachekeys.

Nicht zulässig:

- sichtbare `unordered_map`-Reihenfolge;
- zufällige IDs;
- Zeitstempel im fachlichen Vergleich;
- plattformabhängige Pfadseparatoren in Repository-URIs;
- unterschiedliche Groß-/Kleinschreibung in MD5.

---

## 32. Cancellation

### 32.1 JavaScript

`resolveWorkspace` SOLL `AbortSignal` unterstützen.

Cancellation:

- beendet neue Transportrequests;
- erzeugt `ILIC-REPO-CANCELLED`;
- schreibt keinen unvollständigen Cacheeintrag;
- lässt bestehende vollständige Cacheeinträge unverändert;
- entfernt In-Flight-Promises;
- liefert keine scheinbar erfolgreiche partielle Workspace.

### 32.2 C++

Optionaler Port:

```cpp
class RepositoryCancellation {
public:
    virtual ~RepositoryCancellation() = default;
    virtual bool cancelled() const noexcept = 0;
};
```

Wenn bereits ein allgemeines Cancellation-Modell im Projekt vorhanden ist, soll dieses verwendet werden.

---

## 33. Observability

### 33.1 Ereignisse

Additiv kann ein Observer verwendet werden:

```cpp
struct RepositoryEvent {
    enum class Kind {
        CacheHit,
        CacheMiss,
        CacheStale,
        DownloadStart,
        DownloadSuccess,
        DownloadFailure,
        IndexParsed,
        SiteVisited,
        ModelSelected,
        DependencyResolved,
        WorkspaceInstalled
    };

    Kind kind;
    std::string uri;
    std::string model;
};
```

### 33.2 Regeln

- Observer darf Fachablauf nicht verändern;
- Observerfehler werden ignoriert oder als Debuglog behandelt;
- keine Sourceinhalte loggen;
- keine Credentials;
- keine vollständigen Querytokens;
- deterministische Tests dürfen Observerereignisse prüfen.

---

## 34. Thread Safety und Single Flight

### 34.1 Native Manager

Der Vertrag muss dokumentiert werden.

Bevorzugt:

- öffentliche `resolve`- und `listModels`-Calls auf derselben Instanz sind thread-safe;
- per-Manager-Mutex;
- keine Prozessmutex;
- Cacheadapter selbst thread-safe;
- unterschiedliche Managerinstanzen parallel;
- gleiche Resource wird pro Manager single-flight geladen.

Mindestens muss P4 beweisen:

- zwei Manager parallel;
- zwei Resolve-Calls derselben Instanz;
- zwei Prozesse/Threads schreiben denselben Cachekey;
- keine Race unter TSan.

### 34.2 JavaScript

- In-Flight-Map pro Manager;
- key = Operation + normalisierte URI + relevante Bypassoption;
- fulfilled/rejected Promise wird entfernt oder in Catalogzustand überführt;
- ein Consumer-Abort darf geteilten Request nur abbrechen, wenn keine weiteren Consumer warten;
- einfachere Alternative: kein geteiltes AbortSignal, Caller Cancellation stoppt nur Weiterverarbeitung; dokumentieren.

---

## 35. Contract-Fixtures

### 35.1 Ziel

Native und JavaScript werden über dieselben Fixtures geprüft.

### 35.2 Verzeichnis

```text
test/repository/contract/
  contract-schema.json
  uri-cases.json
  path-cases.json
  xml-cases.json
  version-cases.json
  traversal-cases.json
  dependency-cases.json
  cache-policy-cases.json
  scenarios/
    basic/
    parents/
    subsidiaries/
    cycles/
    offline/
    stale/
    checksum/
    malformed/
```

### 35.3 Scenario-Format

Beispiel:

```json
{
  "schemaVersion": 1,
  "name": "dependency order",
  "repositories": ["repo://root/"],
  "request": {
    "models": ["Root"],
    "schemaLanguage": "ili2_3"
  },
  "clock": 1000000,
  "resources": {
    "repo://root/ilimodels.xml": {
      "status": 200,
      "bodyFile": "ilimodels.xml"
    },
    "repo://root/root.ili": {
      "status": 200,
      "bodyFile": "root.ili"
    }
  },
  "expected": {
    "success": true,
    "models": ["Dependency", "Root"],
    "diagnosticCodes": []
  }
}
```

### 35.4 Canonical JSON

Native und Node Runner müssen dieselbe Canonicalform erzeugen:

- UTF-8;
- feste Feldreihenfolge oder struktureller Vergleich;
- keine absoluten Temp-Pfade;
- keine reale Uhr;
- keine Prozess-ID;
- keine zufälligen Dateinamen;
- lokale Pfade durch stabile Platzhalter normalisieren.

### 35.5 Vergleich

`scripts/compare-repository-contract.mjs`:

1. führt nativen Runner aus;
2. führt JavaScript Runner aus;
3. liest JSON;
4. vergleicht strukturell;
5. schreibt verständlichen Diff;
6. scheitert bei Abweichung.

---

## 36. Tests: C++ Repository Core

Mindestens folgende Testdateien:

```text
test/repository/core/RepositoryPolicyTest.cpp
test/repository/core/RepositoryUriTest.cpp
test/repository/core/RepositoryVersionSelectorTest.cpp
test/repository/core/RepositoryCatalogTest.cpp
test/repository/core/RepositoryGraphWalkerTest.cpp
test/repository/core/RepositoryDependencyResolverTest.cpp
test/repository/core/RepositoryResourceCoordinatorTest.cpp
test/repository/core/RepositoryDeterminismTest.cpp
test/repository/core/RepositoryConcurrencyTest.cpp
```

### 36.1 Policytest

Prüft:

- Defaults;
- TTL-Umrechnung;
- negative Werte;
- Grenzen;
- Overflow;
- explizite Optionen.

### 36.2 Catalogtest

Prüft:

- lazy load;
- positiver Cache;
- negativer Cache;
- invalidate;
- Cancellation;
- transienter Retry;
- Single Flight;
- getrennte Manager.

### 36.3 Resource Coordinator

Prüft jede Kombination:

- miss/online/success;
- fresh hit;
- stale hit/online/success;
- stale hit/transport fail/stale allowed;
- stale hit/transport fail/stale forbidden;
- offline/fresh;
- offline/stale;
- offline/miss;
- optional 404;
- Pflicht 404;
- cache get failure;
- cache put failure;
- checksum hit;
- checksum cache mismatch + retry success;
- checksum cache mismatch + retry mismatch;
- checksum transport mismatch;
- cancellation;
- max bytes.

### 36.4 Dependency Resolver

Prüft:

- eine Wurzel;
- mehrere Wurzeln;
- geteilte Dependency;
- Diamond;
- Zyklus;
- Deep Limit;
- mehrere Modelle pro Datei;
- widersprüchliche Checksummen;
- INTERLIS;
- nicht gefunden;
- browseOnly;
- deterministische Reihenfolge.

---

## 37. Tests: Native Adapter

Mindestens:

```text
test/repository/native/CurlRepositoryTransportTest.cpp
test/repository/native/LocalFileRepositoryTransportTest.cpp
test/repository/native/FileRepositoryCacheTest.cpp
test/repository/native/LibXmlRepositoryMetadataDecoderTest.cpp
test/repository/native/RepositoryManagerIntegrationTest.cpp
test/repository/native/RepositoryCompilerBridgeTest.cpp
```

Tests dürfen kein öffentliches Internet benötigen.

Verwende:

- lokalen HTTP-Testserver;
- temporäres Verzeichnis;
- Fake Clock;
- Fault Injection;
- read-only Cache;
- Redirectserver;
- absichtlich langsame Response;
- große Response;
- kaputtes XML.

---

## 38. Tests: JavaScript Core

Mindestens:

```text
packages/repository-core/test/policy.test.mjs
packages/repository-core/test/resource-coordinator.test.mjs
packages/repository-core/test/catalog.test.mjs
packages/repository-core/test/traversal.test.mjs
packages/repository-core/test/dependency-resolver.test.mjs
packages/repository-core/test/xml.test.mjs
packages/repository-core/test/contract.test.mjs
packages/repository-core/test/concurrency.test.mjs
```

### 38.1 Keine Plattformglobals

Coretests laufen in einer Umgebung, in der:

```js
globalThis.fetch = undefined;
globalThis.indexedDB = undefined;
```

Der Core muss mit injizierten Fakes funktionieren.

### 38.2 Deterministische Uhr

Kein Test darf auf reale Wartezeiten angewiesen sein.

Nicht zulässig:

```js
await new Promise(resolve => setTimeout(resolve, 1100));
```

Stattdessen ManualClock.

### 38.3 Tests der `@ilic/tools`-Compatibility-Fassade

Mindestens:

```text
packages/tools/test/legacy-imports.test.mjs
packages/tools/test/legacy-constructor.test.mjs
packages/tools/test/legacy-load-option.test.mjs
packages/tools/test/default-fetch.test.mjs
packages/tools/test/no-default-transport.test.mjs
packages/tools/test/memory-cache-compatibility.test.mjs
packages/tools/test/resolved-workspace-shape.test.mjs
packages/tools/test/public-api.test.mjs
packages/tools/test/browser-bundle-boundary.test.mjs
```

Die Tests MÜSSEN nachweisen:

- `RepositoryManager` ist eine Fassade und delegiert an den Core;
- ein alter Consumer importiert nicht versehentlich `RepositoryManagerCore`;
- `new RepositoryManager()` konstruiert den bisherigen Defaultcache;
- `load` wird in einen Transport adaptiert;
- `transport` hat Vorrang vor `load`;
- Default-Fetch liegt nur in der Fassade beziehungsweise im Adapter;
- bei fehlendem Transport entsteht eine verständliche Fehlermeldung;
- `new MemoryCache()` funktioniert ohne Argumente;
- bestehende Cacheimplementierungen bleiben strukturell kompatibel;
- die Workspaceform bleibt unverändert;
- der Haupt-Entry-Point enthält keine statischen Node-Builtins;
- der TypeScript-Public-API-Diff ist kompatibel.

---

## 39. Tests: Node

Mindestens:

- NodeFileCache read/write;
- Concurrent writer;
- corrupt metadata;
- corrupt content;
- legacy layout;
- cache migration;
- read-only;
- cleanup;
- file transport;
- local root escape;
- HTTP local server;
- Abort;
- max bytes;
- high-level compatibility import;
- `new RepositoryManager({ repositories })` mit lokalem Fake-Fetch;
- bestehende `load`-Option;
- `createNodeRepositoryManager` als additive API;
- Node-LSP-Consumer-Fixture oder echter Consumer-Smoke.

Befehle müssen unter Node 22 und mindestens der im Package deklarierten Mindestversion funktionieren, sofern CI diese Version unterstützt.

---

## 40. Tests: Browser

Mindestens:

- BrowserCache get/put/delete/clear;
- IndexedDB upgrade;
- Quota failure;
- multiple instances;
- Uint8Array;
- clock;
- Fetch transport;
- CORS-like rejection via mock;
- Abort;
- max bytes;
- high-level RepositoryManager;
- resolved workspace into compiler-wasm;
- P3 editor snapshot nach Workspaceinstallation;
- bestehende Konstruktion ohne Browserfactory;
- additive Browserfactory;
- Production-Bundle ohne `node:`-Imports;
- `interlis-web-ide` Typecheck, Tests und Build mit lokalem Snapshot.

Bevorzugt:

- echter Headless Browser;
- keine reine Node-Simulation für alle Browserfälle.

---

## 41. Compiler-Bridge-Tests

### 41.1 Kein `localPath`

Erzeuge:

```cpp
ResolvedModel model;
model.uri = "https://example.invalid/models/A.ili";
model.source = validSource;
model.localPath.clear();
```

Installation und Kompilierung müssen erfolgreich sein.

### 41.2 Abweichender Dateicontent

Lege absichtlich eine lokale Datei mit anderem Inhalt an und setze `localPath` darauf.

Der Compiler muss weiterhin `model.source` verwenden.

### 41.3 Cache-Write-Failure

Transport erfolgreich, Cache nicht schreibbar:

- RepositoryResult erfolgreich;
- Warnung vorhanden;
- Compiler erfolgreich;
- kein Tempfile erforderlich.

### 41.4 URI-Diagnose

Compilerdiagnosen müssen die Repository-URI enthalten, nicht einen Cachehashpfad.

### 41.5 Imports

Ein Rootmodell importiert ein Repositorymodell:

- beide Sources im `CompilationSourceStore`;
- `IliFileCatalog::loadByModel` findet Repositorysource;
- keine Dateisystemsuche notwendig;
- Compilerresultat identisch zum lokalen Dateiszenario.

### 41.6 Sessionisolation

Zwei Sessions lösen gleiche URI mit unterschiedlichen Fake-Inhalten:

- keine Sourcevermischung;
- getrennte Results;
- kein globaler Repositorycache im Compilerkern;
- TSan sauber.

---

## 42. P1-Conformance

Die externe Suite bleibt verbindlich:

```text
https://codeberg.org/edigonzales/interlis-compiler-testsuite
```

Lokaler Standardpfad:

```text
../interlis-compiler-testsuite
```

Der Agent MUSS den tatsächlichen Aufruf aus `.github/workflows/ci.yml` lesen.

P4 darf:

- keine Baseline erweitern;
- keine Suitefälle überspringen;
- keine neue interne Compilerabweichung erzeugen;
- keine Repositoryänderung als Grund verwenden, Compilersemantik zu lockern.

Mindestens ausführen:

1. vor P4;
2. nach direkter Sourceintegration;
3. nach CMake-Targetumbau;
4. nach JavaScript-Paketumbau;
5. final Release;
6. final Debug;
7. final ASan/UBSan/LSan;
8. TSan gemäß bestehendem Workflow.

Der Conformance-Build mit `ILIC_ENABLE_NATIVE_REPOSITORY=OFF` muss weiter funktionieren.

Zusätzlich benötigt P4 einen Repository-End-to-End-Build mit `ILIC_ENABLE_NATIVE_REPOSITORY=ON`.

---

## 43. P2-Erhalt

P4 DARF NICHT:

- globale Repositorymanager im Compiler einführen;
- globalen Cachezustand in `ilic-core` einführen;
- globale Transportinstanz einführen;
- `thread_local` aktiven Repositorykontext einführen;
- globale Compiler-Mutex wieder einführen;
- Sources zwischen Sessions teilen;
- Repositorydiagnosen über globalen Logger ausgeben.

Alle Abhängigkeiten müssen instanzgebunden sein.

---

## 44. P3-Erhalt

P4 DARF NICHT:

- JavaScript-INTERLIS-Parser wieder einführen;
- EditorSnapshot aus Repositorymetadaten ableiten;
- Source im Repositorymanager lexen;
- Compiler-WASM-Netzwerkzugriff einführen;
- Workerprotokoll unnötig ändern;
- `session.putWorkspace` brechen;
- Native/WASM-Snapshot-Parität schwächen.

Nach einem aufgelösten Workspace muss P3 weiter funktionieren:

```ts
session.putWorkspace(workspace);
session.compile({ roots: [workspace.models.at(-1).uri] });
session.editorSnapshot(workspace.models.at(-1).uri);
```

---

## 45. CMake-Zielstruktur

Bevorzugt:

```cmake
add_library(ilic-repository-core STATIC ...)
add_library(ilic::repository-core ALIAS ilic-repository-core)

add_library(ilic-repository-native STATIC ...)
add_library(ilic::repository ALIAS ilic-repository-native)
```

`ilic-repository-core`:

- kein cURL;
- kein CLI;
- keine konkrete Diskcacheimplementierung;
- C++17;
- testbar mit Fakes.

`ilic-repository-native`:

- verlinkt Core;
- verlinkt cURL/XML;
- enthält Native Adapter und Fassade.

`repository-compiler-bridge` kann:

- Teil von `ilic-repository-native`;
- oder eigenes Target

sein.

`ilic-core` bleibt repositoryfrei.

### 45.1 Buildvarianten

Müssen funktionieren:

```text
ILIC_ENABLE_NATIVE_REPOSITORY=OFF
ILIC_ENABLE_NATIVE_REPOSITORY=ON
ILIC_STATIC_DISTRIBUTION=OFF
ILIC_STATIC_DISTRIBUTION=ON
Debug
Release
ASan/UBSan
TSan
Windows static
macOS
Linux
Emscripten
```

### 45.2 Emscripten

Der Compiler-WASM-Build muss nicht `ilic-repository-native` enthalten.

Das JavaScript-Repository-Core-Paket bleibt der asynchrone Resolver vor `putWorkspace`.

---

## 46. npm-Pakete, Exports und Publishing

Wenn `@ilic/repository-core` neu veröffentlicht wird:

- `scripts/prepare-npm-snapshot.mjs` erweitern;
- Packageversion exakt zur Projektversion;
- explizite `files`;
- README;
- LICENSE;
- Exports;
- Typen;
- Snapshotconsumer-Test;
- kein ungewolltes Node-Modul im Browserbundle;
- keine Browserglobals im Nodebundle.

### 46.1 Rolle von `@ilic/repository-core`

`@ilic/repository-core` ist eine additive Low-Level-Abhängigkeit. Es exportiert `RepositoryManagerCore` beziehungsweise einen äquivalent eindeutig benannten Core-Typ. Es ist nicht der automatische Ersatz für die öffentliche API von `@ilic/tools`.

### 46.2 Rolle von `@ilic/tools`

`@ilic/tools` bleibt veröffentlicht und enthält die echte Compatibility Facade. Es darf reine Funktionen und Typen aus `@ilic/repository-core` re-exportieren, MUSS aber `RepositoryManager` und `MemoryCache` selbst mit ihren bisherigen Defaults und Legacyadaptionen bereitstellen.

`package.json` von `@ilic/tools` MUSS `@ilic/repository-core` als reguläre Laufzeitabhängigkeit oder als im Snapshot korrekt aufgelöste Workspaceabhängigkeit deklarieren.

### 46.3 Subpath-Exports

Folgende Pfade bleiben vorhanden:

```text
@ilic/tools
@ilic/tools/node
@ilic/tools/browser
```

Additiv möglich:

```text
@ilic/repository-core
@ilic/tools/core
```

Ein Consumer darf nicht gezwungen werden, sofort auf einen neuen Paketnamen oder eine Factory zu wechseln.

### 46.4 Snapshot-Publishing

Der Snapshotprozess MUSS alle zusammengehörigen Pakete aus demselben Build veröffentlichen und testen. Insbesondere dürfen nicht kombiniert werden:

- neues `@ilic/tools` mit fehlendem `@ilic/repository-core`;
- neue TypeScript-Typen mit altem Runtimepaket;
- Browseradapter aus einem anderen Snapshot als die Fassade;
- inkompatible Compiler-WASM- und Tools-Snapshots.

### 46.5 Semver und API-Diff

P4 ist für High-Level-Consumer als kompatible, additive Änderung zu behandeln. Ein Major-Version-Sprung darf nicht als Ersatz für die geforderte Kompatibilität verwendet werden. Jede unbeabsichtigte Breaking Change muss im API-Diff fehlschlagen.

---

## 47. Architektur-Guard

Füge `scripts/check-repository-layering.py` hinzu.

Der Guard MUSS mindestens erkennen:

### 47.1 JavaScript Core

Verboten in `packages/repository-core`:

- `node:`;
- `process.`;
- `window.`;
- `document.`;
- `indexedDB`;
- direkter globaler `fetch`;
- `Date.now`;
- `setTimeout` für TTL;
- Compiler-WASM-Imports;
- Regex-basierter kompletter XML-Parser;
- eigene zweite RepositoryManager-Monolithdatei.

Zusätzlich MUSS der Guard für `packages/tools` erkennen:

- bloßen Re-export von `RepositoryManager` aus `@ilic/repository-core`;
- bloßen Re-export von `MemoryCache` aus `@ilic/repository-core`;
- statischen Import von `node:` im universellen Haupt-Entry-Point;
- Entfernung der bisherigen Exportnamen;
- direkte fachliche Repositoryauflösung in der Fassade statt Delegation an den Core.

### 47.2 C++ Core

Verboten in `source/repository/core`:

- `<curl/...>`;
- `CurlRepositoryTransport`;
- direkte Datei-I/O-Streams;
- konkrete Cachepfade;
- CLI-Includes;
- `exit(`;
- globale Mutable State;
- `thread_local`;
- `CompilerContext`;
- `IliFileCatalog`.

### 47.3 Compilerbridge

Verbotenes Muster:

```cpp
loadByFile(model.localPath.string())
```

für Repositoryinstallation.

### 47.4 Doppelimplementierungen

Der Guard soll bekannte alte Symbole verhindern:

- monolithische `#resource`;
- monolithische `#findModel`;
- duplizierter JS-MD5-Code;
- direkte XML-RegEx-Funktionen `tag`, `values`, `decodeXml` in RepositoryManager;
- direkter Default-Fetch im Core.

### 47.5 Canary

Der Agent MUSS:

1. temporär eine verbotene Abhängigkeit einfügen;
2. Guard ausführen;
3. erwartetes Fehlschlagen bestätigen;
4. Canary entfernen;
5. Guard erneut erfolgreich ausführen.

---

## 48. CI

### 48.1 Bestehende Jobs erhalten

- Native Release Matrix;
- Debug;
- ASan/UBSan/LSan;
- TSan;
- P1 Conformance;
- WASM/npm;
- P3 Parität.

### 48.2 Neue Jobs/Schritte

Mindestens:

1. Repository Core C++ Tests ohne Native Adapter.
2. Native Repository Adapter Tests.
3. JavaScript Repository Core Tests.
4. Node Adapter Tests.
5. Browser Adapter Tests.
6. Native/JS Contract Parity.
7. Read-only/no-materialization Compiler Bridge.
8. Architecture Guard.
9. npm Compatibility Consumer.
10. Repository-enabled CLI Integration.
11. Public TypeScript API Diff.
12. Legacy-constructor and Legacy-`load` Contract.
13. `@ilic/tools` Facade Guard.
14. Node-LSP Consumer Smoke.
15. `interlis-web-ide` Typecheck, Test und Production Build.
16. Browser-Bundle-Prüfung auf versehentliche `node:`-Imports.

### 48.3 Kein öffentliches Netz

CI-Repositorytests dürfen nicht von `models.interlis.ch` oder einem anderen Live-Dienst abhängen.

Verwende lokalen HTTP-Server und Fixtures.

### 48.4 Fail-fast

Keine P4-Prüfung darf mit `continue-on-error` dauerhaft ignoriert werden.

Reportupload darf das bestehende Muster verwenden, wenn danach der ursprüngliche Exitcode erzwungen wird.

---

## 49. Migrationsstrategie

### P4.0 – Inventar und Baseline

- Gitstatus;
- aktueller Commit;
- P0–P3 prüfen;
- native Repositorytests;
- npm-Tools-Tests;
- Web-IDE-/Node-LSP-Smoke;
- P1-Conformance;
- öffentliche API erfassen;
- Cachelayouts dokumentieren;
- Diagnosecodes erfassen.

### P4.1 – Contract-Fixtures

- gemeinsame JSON-Schemas;
- vorhandenes Verhalten als Fixtures;
- Native Runner;
- JS Runner;
- zunächst bekannte Abweichungen dokumentieren;
- keine neue Baseline für unerwünschte Drift.

### P4.2 – Ports und Policy

- Clock;
- Transport;
- Cache;
- MetadataDecoder;
- Checksum;
- Policy;
- Fakeports;
- Unit-Tests.

### P4.3 – Resource Coordinator

- Cacheentscheidung extrahieren;
- Offline;
- Stale;
- Checksumretry;
- Fehlerklassifikation;
- Tests.

### P4.4 – Catalog

- Index/Site State;
- Invalidation;
- Single Flight;
- Tests.

### P4.5 – Traversierung und Version

- Graph Walker;
- Version Selector;
- deterministische Fixtures;
- Native/JS Vergleich.

### P4.6 – Dependency Resolver

- DFS;
- Zyklen;
- Deduplizierung;
- Limits;
- Workspace;
- Tests.

### P4.7 – Native Adapter

- cURL;
- lokale Datei;
- Diskcache;
- libxml;
- System Clock;
- Fassade.

### P4.8 – Direkte Compilerintegration

- SourceOrigin Repository;
- rememberRepositorySource;
- WorkspaceInstaller;
- CLI umstellen;
- Tempfile-Zwang entfernen;
- Bridge-Tests;
- P1-Conformance.

### P4.9 – JavaScript Core

- `@ilic/repository-core`;
- Monolith aufteilen;
- Regex-XML entfernen;
- MD5-Port;
- Legacyoptionen adaptieren;
- Coretests.

### P4.10 – Node/Browser Adapter

- NodeFileCache;
- Node Transport;
- BrowserCache;
- Fetch Transport;
- Factories;
- Migration;
- Browsertests.

### P4.11 – Compatibility Facade

- `RepositoryManagerCore` eindeutig benennen;
- echten `@ilic/tools`-Wrapper implementieren;
- `MemoryCache`-Wrapper mit Defaultuhr implementieren;
- reine Funktionen und Typen gezielt re-exportieren;
- alte Imports erhalten;
- alte Konstruktoroptionen erhalten;
- alte Cachetypen erhalten;
- alte `load`-Option über Adapter erhalten;
- Default-Fetch im Facade-Layer erhalten;
- `ResolvedWorkspace`-Form erhalten;
- `onWarning` erhalten;
- compiler-wasm `putWorkspace` erhalten;
- Public-API-Diff ausführen;
- Legacy-Fixtures kompilieren;
- Node-LSP ohne Produktionscodeänderung testen;
- Web-IDE ohne Produktionscodeänderung testen.

### P4.12 – Guards und CI

- Layering Guard;
- Canary;
- Contract Parity;
- CI-Jobs.

### P4.13 – Abschluss

- Release;
- Debug;
- Sanitizer;
- TSan;
- WASM;
- npm;
- Browser;
- Consumer;
- Conformance;
- Dokumentation;
- Gitstatus.

---

## 50. Verbotene Abkürzungen

Folgendes ist ausdrücklich nicht zulässig:

- nur Dateien umbenennen;
- `RepositoryManager` als Monolith behalten und Ports nur als Wrapper davor setzen;
- globale Service-Locator;
- Singleton-Cache im Compilerkern;
- `thread_local RepositoryManager`;
- Compilerkern abhängig von cURL;
- Browsercache im Repository-Core;
- Node-Dateisystem im Repository-Core;
- `Date.now()` im Core;
- Regex als XML-Parser;
- stiller Fallback auf altes JS-Verhalten;
- Native und JS ohne Paritätstest;
- Compiler weiterhin aus `localPath` lesen;
- Tempfile als Pflicht;
- Cache-Write-Fehler fatal machen;
- Stale-Inhalt bei Prüfsummenfehler akzeptieren;
- Prüfsummen abschalten, um Tests grün zu machen;
- Sicherheitslimits unendlich setzen;
- Live-Internettests;
- P1-Baseline erweitern;
- P2-Globals wiedereinführen;
- P3-API brechen;
- `RepositoryManager` aus `@ilic/repository-core` bloß als Ersatz für die bisherige `@ilic/tools`-Klasse re-exportieren;
- `MemoryCache` ohne Erhalt des parameterlosen Konstruktors ersetzen;
- bestehende Consumer zur Verwendung von `RepositoryManagerCore` zwingen;
- bestehende Consumer zur Verwendung von `createNodeRepositoryManager` oder `createBrowserRepositoryManager` zwingen;
- die `load`-Option entfernen oder semantisch ignorieren;
- den Default-HTTP-/HTTPS-Fetchpfad von `new RepositoryManager({ repositories })` entfernen;
- `ResolvedWorkspace` inkompatibel ändern;
- `localPath` für JavaScript-Consumer verpflichtend machen;
- statische Node-Imports in den universellen `@ilic/tools`-Entry-Point aufnehmen;
- Node-LSP-/Web-IDE-Produktionscode ändern, um eine P4-Regression zu kaschieren;
- Tests löschen oder Erwartungen lockern;
- Leak Detection abschalten;
- TSan-Fehler unterdrücken;
- `continue-on-error`;
- C++20 einführen;
- fremde Änderungen überschreiben.

---

## 51. Definition of Done

P4 ist nur abgeschlossen, wenn:

- Repository-Core klar getrennt ist;
- Ports explizit sind;
- Core keine Plattformglobals verwendet;
- Native Adapter getrennt sind;
- Node Adapter getrennt sind;
- Browser Adapter getrennt sind;
- XML-RegEx entfernt ist;
- Uhr injiziert ist;
- Cachepolicy zentral ist;
- Stale-/Offline-Regeln getestet sind;
- Checksumretry getestet ist;
- Site-Traversierung deterministisch ist;
- Versionwahl deterministisch ist;
- Dependencyauflösung deterministisch ist;
- Native/JS Contract Parity grün ist;
- `ResolvedModel.source` autoritativ ist;
- Compiler ohne `localPath` kompiliert;
- Cache-Write-Failure Kompilierung nicht verhindert;
- Repository-URI in Compilerdiagnosen erscheint;
- `@ilic/repository-core` einen eindeutig benannten Low-Level-Manager exportiert;
- `@ilic/tools` eine echte Fassade und keinen bloßen Manager-Re-export enthält;
- `new RepositoryManager({ repositories })` für HTTP-/HTTPS-Repositories weiter funktioniert;
- die bestehende `load`-Option weiter funktioniert;
- `new MemoryCache()` weiter funktioniert;
- `@ilic/tools`, `@ilic/tools/node` und `@ilic/tools/browser` kompatibel bleiben;
- der Public-TypeScript-API-Diff keine unbeabsichtigte Breaking Change zeigt;
- die Form von `ResolvedWorkspace` kompatibel bleibt;
- `session.putWorkspace` kompatibel bleibt;
- Node-LSP-Smoke ohne fachliche Produktionscodeänderung grün ist;
- Web-IDE-Typecheck, Tests und Production Build ohne fachliche Produktionscodeänderung grün sind;
- das Browserbundle keine versehentlichen Node-Imports enthält;
- P1-Conformance keine neue Abweichung zeigt;
- P2-Concurrency erhalten ist;
- P3 Native/WASM-Parität erhalten ist;
- Release grün ist;
- Debug grün ist;
- ASan/UBSan/LSan grün sind;
- TSan grün ist;
- Browsertests grün sind;
- Architecture Guard grün ist;
- Canary nachgewiesen ist;
- Dokumentation aktualisiert ist;
- keine fremden Änderungen beschädigt sind.

---

## 52. Verifikationsbefehle

Der Agent muss die tatsächlichen Projektnamen anpassen.

### 52.1 Release

```bash
cmake -S . -B build/p4-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=ON \
  -DILIC_STATIC_DISTRIBUTION=OFF

cmake --build build/p4-release --parallel

ctest --test-dir build/p4-release --output-on-failure
```

### 52.2 Repository disabled

```bash
cmake -S . -B build/p4-no-repository -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF

cmake --build build/p4-no-repository --parallel
ctest --test-dir build/p4-no-repository --output-on-failure
```

### 52.3 Debug

```bash
cmake -S . -B build/p4-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=ON

cmake --build build/p4-debug --parallel
ctest --test-dir build/p4-debug --output-on-failure
```

### 52.4 Sanitizer

```bash
cmake -S . -B build/p4-sanitizers -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=ON \
  -DILIC_ENABLE_SANITIZERS=ON

cmake --build build/p4-sanitizers --parallel

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build/p4-sanitizers --output-on-failure -j2
```

### 52.5 TSan

```bash
cmake -S . -B build/p4-tsan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=ON \
  -DILIC_ENABLE_THREAD_SANITIZER=ON

cmake --build build/p4-tsan --parallel

TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1 \
ctest --test-dir build/p4-tsan \
  -R "Repository|CompilerConcurrency|CapiConcurrency" \
  --output-on-failure
```

### 52.6 npm

```bash
npm test --prefix packages/repository-core
npm test --prefix packages/tools
npm test --prefix packages/compiler-wasm
node --test test/npm/PrepareNpmSnapshotTest.mjs
node scripts/prepare-npm-snapshot.mjs --timestamp 20260101000000
node scripts/test-npm-packages.mjs
```

### 52.7 Contract Parity

```bash
node scripts/compare-repository-contract.mjs \
  --native build/p4-release/ilic-repository-contract-runner \
  --js packages/repository-core/test/run-contract.mjs \
  --fixtures test/repository/contract
```

### 52.8 Architecture Guard

```bash
python3 scripts/check-repository-layering.py .
```

### 52.9 Wiederholung

```bash
ctest --test-dir build/p4-debug \
  --repeat until-fail:20 \
  -R "Repository.*(Concurrency|Cache|Contract|CompilerBridge)" \
  --output-on-failure
```

### 52.10 Public-API- und Legacy-Kompatibilität

Passe die tatsächlichen Skriptnamen an. Mindestens äquivalente Prüfungen sind verpflichtend:

```bash
node scripts/check-public-api.mjs \
  --before build/p4-baseline-api.json \
  --after packages/tools/index.d.ts

node --test test/npm/ToolsLegacyApiCompatibilityTest.mjs
node --test test/npm/RepositoryLegacyLoadOptionTest.mjs
node --test test/npm/RepositoryFacadeDefaultFetchTest.mjs
node --test test/npm/RepositoryFacadeNoTransportTest.mjs
```

### 52.11 Node-LSP-Consumer

Wenn ein lokaler Checkout vorhanden ist, verwende ein gepacktes P4-Snapshotpaket in einem temporären Worktree oder einer anderweitig reversiblen Testinstallation. Führe die tatsächlich vorhandenen Befehle aus, mindestens Typecheck und Tests. Der Consumer-Produktionscode darf nicht zur Kompensation geändert werden.

Beispielhaft:

```bash
pnpm install --frozen-lockfile
pnpm typecheck
pnpm test
```

### 52.12 `interlis-web-ide`

Teste die zusammengehörigen P4-Snapshotpakete in einem sauberen oder temporären Checkout:

```bash
pnpm install --frozen-lockfile
pnpm typecheck
pnpm test
pnpm build
```

Zusätzlich MUSS ein Bundle-Scan nach unbeabsichtigten Node-Builtins erfolgen.

### 52.13 Abschluss

```bash
git diff --check
git status --short
```

---

## 53. Abschlussbericht

Der Coding-Agent muss einen Bericht mit exakt diesen Abschnitten liefern:

1. **Ausgangszustand**
2. **Verwendeter Commit**
3. **P0–P3-Voraussetzungen**
4. **Repository-Inventar vor P4**
5. **Zielarchitektur**
6. **Repository-Core**
7. **Port-Schnittstellen**
8. **RepositoryPolicy**
9. **Clock**
10. **Transport**
11. **Cache**
12. **Metadaten-Decoder**
13. **Prüfsummen**
14. **Resource Coordinator**
15. **Catalog**
16. **Site-Graph-Traversierung**
17. **Versionswahl**
18. **Dependency Resolver**
19. **Native Adapter**
20. **Node Adapter**
21. **Browser Adapter**
22. **Direkte Compilerintegration**
23. **Entfernte Tempfile-Abhängigkeit**
24. **Öffentliche C++-API**
25. **Öffentliche JavaScript-/TypeScript-API**
26. **`RepositoryManagerCore`-Low-Level-API**
27. **`@ilic/tools`-Compatibility-Fassade**
28. **Legacy-Konstruktor, `load` und Default-Fetch**
29. **Public-TypeScript-API-Diff**
30. **`@ilic/tools`-Kompatibilität**
31. **Compiler-WASM-Kompatibilität**
32. **Node-LSP-Kompatibilität**
33. **Web-IDE-Kompatibilität**
34. **Contract-Fixtures**
35. **Native/JavaScript-Parität**
36. **Security**
37. **Concurrency und Single Flight**
38. **Neue und geänderte Dateien**
39. **Neue Tests**
40. **Release-Ergebnis**
41. **Repository-disabled-Ergebnis**
42. **Debug-Ergebnis**
43. **ASan-/UBSan-/LSan-Ergebnis**
44. **TSan-Ergebnis**
45. **WASM-/npm-Ergebnis**
46. **Browser-Ergebnis**
47. **P1-Conformance**
48. **P3-Parität**
49. **Architecture Guard**
50. **Ausgeführte Befehle**
51. **Abweichungen von der Spezifikation**
52. **Externe Blocker**
53. **Verbleibende Risiken**
54. **Abschließender Git-Status**

Für jeden Lauf:

- exakter Befehl;
- Exitcode;
- Testanzahl;
- Fehleranzahl;
- relevante Umgebung;
- Reportpfad;
- Suite-Commit und Baseline bei Conformance.

Keine erfolgreiche Behauptung ohne tatsächlichen Lauf.

---

## 54. Empfohlene Commitfolge

1. `test: capture repository contract fixtures`
2. `refactor: introduce repository ports and policy`
3. `refactor: extract repository resource coordinator`
4. `refactor: extract catalog and graph traversal`
5. `refactor: extract dependency resolver`
6. `refactor: split native repository adapters`
7. `refactor: install resolved sources without materialization`
8. `refactor: add platform-neutral repository JS core`
9. `refactor: split node and browser adapters`
10. `test: enforce native-js repository parity`
11. `test: add repository layering guard`
12. `ci: verify repository architecture and consumers`
13. `docs: document P4 repository architecture`

Der Agent muss nicht committen, wenn der Auftrag keine Git-Publikation verlangt. Die Sequenz beschreibt reviewbare Einheiten.

---

## 55. Kompakter Startprompt für einen Agenten

```text
Arbeite P4 anhand der Datei ilic-p4-repository-modularization-spec-v2.md vollständig ab.

Beginne mit git status --short, git branch --show-current und git rev-parse HEAD.
Bewahre fremde Änderungen. Lies die Spezifikation sowie alle Repository-, Compiler-
Bridge-, npm-, CMake-, CI- und Consumerdateien vollständig.

Erfasse vor Änderungen Release, CTest, Repositorytests, npm, Browser, P1-Conformance,
P2-Concurrency und P3-Parität. Implementiere danach den Repository-Core, explizite
Ports, Native-/Node-/Browseradapter, gemeinsame Contract-Fixtures und die direkte
ResolvedModel.source-Integration ohne Tempfile-Zwang.

Implementiere `@ilic/repository-core` als Low-Level-Core mit eindeutigem Namen
`RepositoryManagerCore` oder äquivalent. Erhalte `RepositoryManager` und `MemoryCache`
in `@ilic/tools` als echte Compatibility Facade; ein bloßer Re-export ist verboten.
Erhalte alte Imports, Konstruktoroptionen, `load`, Default-Fetch, ResolvedWorkspace,
session.putWorkspace, Node-LSP und interlis-web-ide ohne fachliche Produktionscodeänderung.

Erweitere keine Conformance-Baseline. Beende die Arbeit erst nach Release, Debug,
Sanitizern, TSan, WASM/npm, Browser, Public-API-Diff, Legacy-Fixtures, Consumer-Smokes,
Contract-Parität, Conformance, Architecture Guard und vollständigem Abschlussbericht.
```

---

## 56. Explizite Erfolgsaussage

Der Agent darf P4 nur dann als abgeschlossen bezeichnen, wenn folgende Aussage wahr und durch tatsächlich ausgeführte Tests belegt ist:

> Die INTERLIS-Repository-Auflösung besitzt einen klar abgegrenzten fachlichen Kern mit explizit injizierten Plattformports. `@ilic/repository-core` stellt eine eindeutig benannte Low-Level-API bereit; `@ilic/tools` bleibt eine echte rückwärtskompatible High-Level-Fassade und ist kein bloßer Re-export. Native, Node und Browser erfüllen denselben getesteten Repositoryvertrag. Cache-, Offline-, Stale-, Sicherheits-, Versions-, Site- und Dependency-Entscheidungen sind deterministisch. Aufgelöste Modellquellen werden ohne erzwungene lokale Materialisierung direkt in den Compiler übernommen. Bestehende Imports, Konstruktoroptionen, `load`, Default-Fetch, `MemoryCache`, `ResolvedWorkspace`, Compiler-WASM, Node-LSP und `interlis-web-ide` bleiben ohne fachliche Consumer-Migration nutzbar, und P1-Conformance, P2-Isolation sowie P3-Native/WASM-Parität zeigen keine neue Regression.

# Anhang A – Klassen- und Methodenmatrix

Die folgende Matrix ist bei der Umsetzung als Reviewcheckliste zu verwenden.

| Klasse/Modul | Methode | Verantwortung | Darf nicht |
|---|---|---|---|
| `RepositoryPolicy` | `normalize` | Defaults, Limits, Validierung | Transport öffnen |
| `RepositoryUri` | `parse` | URI klassifizieren und normalisieren | Datei lesen |
| `RepositoryUri` | `resolve` | relativen Link sicher auflösen | Netzwerkzugriff |
| `RepositoryClock` | `now` | Zeit liefern | Policy entscheiden |
| `RepositoryTransport` | `get` | Bytes laden | Cache lesen |
| `RepositoryCachePort` | `get` | Cacheeintrag lesen | TTL entscheiden |
| `RepositoryCachePort` | `put` | atomar speichern | Download starten |
| `RepositoryMetadataDecoder` | `decodeModelIndex` | XML zu Domain | Version wählen |
| `RepositoryMetadataDecoder` | `decodeSite` | Site XML zu Domain | Graph traversieren |
| `RepositoryChecksum` | `md5` | Bytes hashen | Source verändern |
| `RepositoryResourceCoordinator` | `load` | Cache/Transport/Offline | XML parsen |
| `RepositoryCatalog` | `index` | Index lazy laden | Modellabhängigkeiten lösen |
| `RepositoryCatalog` | `site` | Site lazy laden | Version wählen |
| `RepositoryGraphWalker` | `traversalOrder` | Site-Reihenfolge | Dateien laden |
| `RepositoryGraphWalker` | `findModel` | Candidate finden | Dependency DFS |
| `ModelVersionSelector` | `select` | Version wählen | Transport |
| `RepositoryDependencyResolver` | `resolve` | Workspace erzeugen | Compiler aufrufen |
| `RepositoryManager` | `resolve` | Fassade/Orchestrierung | CLI-Ausgabe |
| `RepositoryWorkspaceInstaller` | `install` | Source in Compiler installieren | Netzwerkzugriff |
| `NodeFileCache` | `put` | atomare Nodepersistenz | TTL entscheiden |
| `BrowserCache` | `put` | IndexedDB persistieren | Version wählen |
| `FetchRepositoryTransport` | `get` | HTTP im Browser/Node | XML parsen |
| `CurlRepositoryTransport` | `get` | HTTP native | Cachepolicy |

# Anhang B – Zustandsmaschine des Resource Coordinators

## B.1 Zustände

```text
Start
  -> CacheLookup
  -> FreshCache
  -> OfflineCache
  -> Transport
  -> TransportFailure
  -> StaleFallback
  -> ChecksumValidation
  -> BypassRetry
  -> CacheStore
  -> Success
  -> Failure
  -> Cancelled
```

## B.2 Übergänge

| Von | Bedingung | Nach | Effekt |
|---|---|---|---|
| Start | Request invalid | Failure | URI-Diagnose |
| Start | valid | CacheLookup | Cachekey |
| CacheLookup | hit + fresh | ChecksumValidation | fromCache=true |
| CacheLookup | offline + hit | ChecksumValidation | stale entsprechend |
| CacheLookup | offline + miss + optional | Failure/Unavailable | keine Errordiagnose |
| CacheLookup | offline + miss + required | Failure | Offline Error |
| CacheLookup | online und kein fresh hit | Transport | Request |
| Transport | success | CacheStore | Bytes |
| Transport | optional 404 | Failure/Unavailable | kein Error |
| Transport | retryable fail + stale + allowed | StaleFallback | Warnung |
| Transport | sonst fail | Failure | Download Error |
| CacheStore | store success | ChecksumValidation | localPath optional |
| CacheStore | store fail | ChecksumValidation | Cachewarnung |
| ChecksumValidation | no expected checksum | Success | – |
| ChecksumValidation | match | Success | – |
| ChecksumValidation | mismatch + cached + online + not retried | BypassRetry | invalidate |
| ChecksumValidation | mismatch sonst | Failure | Checksum Error |
| BypassRetry | success | ChecksumValidation | retried=true |
| jeder | cancellation | Cancelled | keine partielle Success |

## B.3 Invarianten

- höchstens ein Bypass-Retry;
- kein stale nach Checksumfehler;
- kein Cachewrite für unvollständige Response;
- optional betrifft nur Abwesenheit, nicht Sicherheitsfehler;
- Cachewarnungen ändern `success` nicht;
- Cancellation setzt kein permanentes negatives Catalogresultat.

# Anhang C – Resource-Testmatrix

| ID | Cache/Start | Transport | Erwartung |
|---|---|---|---|
| fresh-cache | fresh hit | kein Transport | success, fromCache |
| stale-online-success | stale hit | Transport erfolgreich | neue Bytes |
| stale-transient-failure | stale hit | retryable Fehler | stale success wenn erlaubt |
| stale-permanent-failure | stale hit | 404 Pflicht | failure |
| offline-fresh | fresh hit | offline | success |
| offline-stale | stale hit | offline | stale success |
| offline-miss | miss | offline | offline error |
| optional-site-404 | miss | 404 | unavailable ohne error |
| required-index-404 | miss | 404 | download/index error |
| cache-get-error | cache throws | transport success | success + warning |
| cache-put-error | transport success | cache write fails | success + warning |
| checksum-match | bytes match | - | success |
| checksum-cached-mismatch-retry-match | cached mismatch | bypass matches | success |
| checksum-cached-mismatch-retry-mismatch | cached mismatch | bypass mismatch | failure |
| checksum-network-mismatch | network mismatch | - | failure |
| size-limit-index | index too large | - | limit error |
| size-limit-model | model too large | - | limit error |
| cancel-before | cancelled before request | - | cancelled |
| cancel-during | abort during transport | - | cancelled |
| future-cache-time | storedAt > now | - | defined warning/policy |

# Anhang D – Traversierungs-Testmatrix

| ID | Setup | Erwartung |
|---|---|---|
| seeds-first | A,B seeds | A then B |
| parent-after-seeds | A links parent P | A then P |
| subsidiary-after-parents | A links P and S | A,P,S |
| parent-from-subsidiary | S links SP | SP according to parent priority |
| site-cycle | A -> B -> A | each once |
| duplicate-links | same URI repeated | each once |
| relative-links | ../parent | normalized safe URI |
| invalid-link | unsafe link | diagnostic, continue |
| limit | more than max sites | limit error |
| no-follow | links present, disabled | seeds only |

# Anhang E – Security-Testmatrix

| Test | Muss |
|---|---|
| absolute POSIX model path | kontrolliert ablehnen, kein Crash, kein Escape |
| Windows drive path | kontrolliert ablehnen, kein Crash, kein Escape |
| UNC path | kontrolliert ablehnen, kein Crash, kein Escape |
| plain .. | kontrolliert ablehnen, kein Crash, kein Escape |
| percent-encoded .. | kontrolliert ablehnen, kein Crash, kein Escape |
| mixed slash/backslash traversal | kontrolliert ablehnen, kein Crash, kein Escape |
| NUL byte | kontrolliert ablehnen, kein Crash, kein Escape |
| embedded URI scheme | kontrolliert ablehnen, kein Crash, kein Escape |
| query string in file path | kontrolliert ablehnen, kein Crash, kein Escape |
| fragment in file path | kontrolliert ablehnen, kein Crash, kein Escape |
| symlink escape | kontrolliert ablehnen, kein Crash, kein Escape |
| redirect HTTPS to file | kontrolliert ablehnen, kein Crash, kein Escape |
| redirect loop | kontrolliert ablehnen, kein Crash, kein Escape |
| too many redirects | kontrolliert ablehnen, kein Crash, kein Escape |
| XXE | kontrolliert ablehnen, kein Crash, kein Escape |
| external DTD | kontrolliert ablehnen, kein Crash, kein Escape |
| Billion Laughs | kontrolliert ablehnen, kein Crash, kein Escape |
| deep XML nesting | kontrolliert ablehnen, kein Crash, kein Escape |
| oversized metadata | kontrolliert ablehnen, kein Crash, kein Escape |
| oversized model | kontrolliert ablehnen, kein Crash, kein Escape |
| invalid UTF-8 | kontrolliert ablehnen, kein Crash, kein Escape |
| checksum mismatch | kontrolliert ablehnen, kein Crash, kein Escape |
| cache metadata path traversal | kontrolliert ablehnen, kein Crash, kein Escape |
| cache generation filename traversal | kontrolliert ablehnen, kein Crash, kein Escape |

# Anhang F – Verbindlicher API- und Consumer-Kompatibilitätsvertrag

## F.1 Unveränderte JavaScript-Imports

Diese Imports müssen weiter kompilieren und laufen:

```ts
import {
  RepositoryManager,
  MemoryCache,
  parseIliModelsXml,
  parseIliSiteXml,
  selectLatestModelVersion,
  supportedSchemaLanguagePreference,
  normalizeRepositoryUri,
  resolveRepositoryUri,
  validateRepositoryRelativePath
} from "@ilic/tools";

import { NodeFileCache } from "@ilic/tools/node";
import { BrowserCache } from "@ilic/tools/browser";
```

## F.2 Bestehender High-Level-Konstruktor

```ts
const manager = new RepositoryManager({
  repositories: ["https://models.interlis.ch"],
  cache: new MemoryCache(),
  load: async uri => new Uint8Array(),
  offline: false,
  metadataTtlMs: 86_400_000,
  modelTtlMs: 604_800_000,
  allowStaleOnError: true,
  followSiteLinks: true,
  validateChecksums: true,
  onWarning: warning => console.warn(warning)
});

const workspace = await manager.resolveWorkspace(["Model"], "ili2_3");
const single = await manager.resolveModel("Model", "ili2_3");
const models = await manager.listModels();
```

Dieser Code darf nicht auf `RepositoryManagerCore` umgestellt werden müssen.

## F.3 Default-Fetch-Kompatibilität

Dieser Code muss in einer unterstützten Browser- oder Node-Laufzeit mit `globalThis.fetch` weiter funktionieren:

```ts
const manager = new RepositoryManager({
  repositories: ["https://example.test/models"]
});
```

Tests verwenden einen Fake- oder lokalen Fetch und kein Live-Internet.

## F.4 `load`-Option

Dieser Code muss weiter funktionieren, ohne dass zusätzlich ein `transport` verlangt wird:

```ts
const manager = new RepositoryManager({
  repositories: ["https://example.test/models"],
  load: async uri => fixtures.get(uri)
});
```

Der `load`-Callback erhält weiterhin die URI als String und darf String oder Bytes zurückgeben.

## F.5 Cache-Kompatibilität

```ts
const memory = new MemoryCache();
await memory.put("key", new Uint8Array([1, 2, 3]));
const entry = await memory.get("key");
await memory.delete("key");
await memory.clear();
```

Bestehende benutzerdefinierte Caches mit dem bisherigen `RepositoryCache`-Interface müssen weiter akzeptiert werden.

## F.6 `ResolvedWorkspace`

```ts
interface ResolvedWorkspace {
  models: Array<{
    metadata: ModelMetadata;
    uri: string;
    source: string;
    fromCache: boolean;
    stale: boolean;
  }>;
}
```

Die fünf bestehenden Modellfelder bleiben vorhanden und behalten ihre Bedeutung. Additive Felder sind optional.

## F.7 Compiler-WASM

```ts
const compiler = await createCompiler();
const session = compiler.createSession();
session.putWorkspace(workspace);
const result = session.compile({
  roots: workspace.models.map(model => model.uri)
});
```

Keine Repository-I/O wird in den synchronen WASM-Compilerkern verschoben.

## F.8 Node-LSP

Der bestehende Integrationsablauf bleibt:

```ts
const workspace = await repositoryManager.resolveWorkspace(
  requestedModels,
  schemaLanguage
);

compilerSession.putWorkspace(workspace);
```

Ein Node-LSP muss nicht:

- `@ilic/repository-core` importieren;
- einen Transportport manuell konstruieren;
- `localPath` verwenden;
- Source einzeln registrieren;
- seine LSP-Protokolle ändern.

## F.9 `interlis-web-ide`

Die Web-IDE darf weiterhin ihre Snapshotpakete verwenden:

```ts
import { RepositoryManager, BrowserCache } from "@ilic/tools";
```

beziehungsweise die heute tatsächlich verwendeten öffentlichen Exporte. P4 darf keine App-Codeänderung erzwingen. PWA- und Service-Worker-Caches müssen zusammengehörige Paketassets aus demselben Snapshot ausliefern.

## F.10 C++

```cpp
ilic::RepositoryOptions options;
options.repositories = {"https://models.interlis.ch"};

ilic::RepositoryManager manager(options);
ilic::RepositoryResult workspace =
    manager.resolve("Model", "ili2_3");
```

Diese Public APIs dürfen nur additiv erweitert werden.

## F.11 Additive neue APIs

Neue Anwendungen dürfen explizit verwenden:

```ts
import { RepositoryManagerCore } from "@ilic/repository-core";
import { createNodeRepositoryManager } from "@ilic/tools/node";
import { createBrowserRepositoryManager } from "@ilic/tools/browser";
```

Diese APIs sind optional und ersetzen den bisherigen High-Level-Vertrag nicht.

## F.12 Verhalten, das präziser werden darf

Folgende Aspekte dürfen nach P4 strenger und sicherer werden:

- unsichere Pfade;
- XXE;
- Größenlimits;
- ungültige URI;
- inkonsistente Checksummen;
- beschädigter Cache;
- gemischte Cachegenerationen.

Solche Änderungen sind im Abschlussbericht aufzulisten und mit Tests zu belegen. Sie dürfen nicht als Vorwand dienen, Paketimports, Konstruktoren oder Workspaceformen zu brechen.

## F.13 Verbindliche Kompatibilitätstests

Mindestens folgende Compile-/Runtime-Fixtures sind erforderlich:

1. alter Hauptimport aus `@ilic/tools`;
2. alter Node-Subpath-Import;
3. alter Browser-Subpath-Import;
4. parameterloser `MemoryCache`;
5. `RepositoryManager` mit `load`;
6. `RepositoryManager` mit benutzerdefiniertem Cache;
7. `RepositoryManager` nur mit Repositories und Fake-`globalThis.fetch`;
8. `onWarning`;
9. `resolveModel`;
10. `resolveWorkspace`;
11. `listModels`;
12. `session.putWorkspace`;
13. Node-LSP-Smoke;
14. Web-IDE-Typecheck/Test/Build;
15. Public-TypeScript-API-Diff.

# Anhang G – Code-Review-Checklisten

## G.1 Repository Core

- [ ] keine Plattformglobalen
- [ ] keine konkrete I/O
- [ ] Clock injiziert
- [ ] deterministische Container
- [ ] keine sichtbare Hashreihenfolge
- [ ] klare Ownership
- [ ] keine globalen Caches
- [ ] Cancellation definiert
- [ ] Limits geprüft
- [ ] Diagnosen strukturiert
- [ ] Contract-Fixtures grün

## G.2 Native Adapter

- [ ] cURL nur im Adapter
- [ ] libxml nur im Decoder
- [ ] Diskcache atomar
- [ ] Windows Rename getestet
- [ ] lokale Root-Containment
- [ ] no-tempfile Compilerbridge
- [ ] ASan/LSan
- [ ] TSan

## G.3 JavaScript Core und Fassade

Core:

- [ ] kein fetch
- [ ] kein indexedDB
- [ ] kein node:
- [ ] kein Date.now
- [ ] keine XML-RegEx
- [ ] keine zweite MD5-Kopie
- [ ] eindeutiger Name `RepositoryManagerCore` oder äquivalent
- [ ] Promise Single Flight
- [ ] Abort sauber
- [ ] Browser/Node Contract gleich

`@ilic/tools`-Fassade:

- [ ] echter Wrapper, kein bloßer Manager-Re-export
- [ ] `RepositoryManager` bleibt exportiert
- [ ] `MemoryCache` bleibt exportiert
- [ ] alte Optionen werden übersetzt
- [ ] `load` funktioniert
- [ ] Default-Fetch funktioniert
- [ ] kein statischer Node-Import im Haupt-Entry-Point
- [ ] `ResolvedWorkspace` bleibt kompatibel
- [ ] Public-API-Diff grün

## G.4 Compilerbridge

- [ ] source autoritativ
- [ ] URI als Diagnosequelle
- [ ] localPath optional
- [ ] Cachefehler nicht fatal
- [ ] keine Source-Lifetime-Lücke
- [ ] Sessionisolation
- [ ] Importauflösung aus SourceStore
- [ ] P1 Conformance

## G.5 Consumer

- [ ] `@ilic/tools` Hauptimports unverändert
- [ ] `@ilic/tools/node` unverändert
- [ ] `@ilic/tools/browser` unverändert
- [ ] alter `RepositoryManager`-Konstruktor
- [ ] alter `MemoryCache`-Konstruktor
- [ ] alte `load`-Option
- [ ] Default-Fetch
- [ ] `ResolvedWorkspace`-Form
- [ ] Node-LSP ohne Produktionscodeänderung
- [ ] Web-IDE ohne Produktionscodeänderung
- [ ] compiler-wasm `putWorkspace`
- [ ] P3 `editorSnapshot`
- [ ] npm snapshot
- [ ] Browser production build
- [ ] PWA-Assetkonsistenz

# Anhang H – Fehler- und Retryklassifikation

| Kategorie | Beispiel | Retry | Stale erlaubt | Diagnose |
|---|---|---:|---:|---|
| DNS | Host nicht auflösbar | ja | ja | DOWNLOAD |
| Timeout | Connect/Read | ja | ja | DOWNLOAD |
| HTTP 500 | Serverfehler | ja | ja | DOWNLOAD |
| HTTP 429 | Rate Limit | optional | ja | DOWNLOAD |
| HTTP 404 Index | fehlt | nein | nein | INDEX |
| HTTP 404 Site | optional | nein | nein/kein Fehler | SITE |
| HTTP 404 Model | fehlt | nein | nein | DOWNLOAD |
| TLS | Zertifikat | nein | policyabhängig, bevorzugt nein | DOWNLOAD |
| Cancellation | Abort | nein | nein | CANCELLED |
| XML malformed | Index kaputt | nein | nein | XML/INDEX |
| Unsafe path | `../` | nein | nein | PATH |
| Checksum | mismatch | einmal bypass | nein | CHECKSUM |
| Cache get | I/O | Transport | – | CACHE warning |
| Cache put | I/O | nein | – | CACHE warning |
| Size limit | zu groß | nein | nein | LIMIT |
| Dependency cycle | A→B→A | nein | nein | CYCLE |
| Model missing | nicht gefunden | nein | nein | NOT-FOUND |
