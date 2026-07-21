# Modell-Repositories

[Dokumentationsindex](README.md) · [CLI](cli.md) · [Build](build-und-installation.md)

`ilic` kann INTERLIS-Modelle aus lokalen Verzeichnissen, `file://`-URIs und
HTTP(S)-Repositories beziehen. Die native C++-Implementierung und
`@ilic/tools` verwenden dabei dieselben Regeln für Suche, Versionen,
Abhängigkeiten, Pfade und Prüfsummen.

## Konfiguration und Priorität

Die Library arbeitet ausschließlich mit den in `RepositoryOptions.repositories`
übergebenen Seed-Repositories. Sie fügt keine versteckten Defaults ein. Die CLI
stellt dagegen `https://models.interlis.ch` als Default bereit. Explizite Werte
aus `-repositories` stehen davor; `--no-default-repositories` unterdrückt den
Default. Trivial identische URIs wie eine HTTP-URL mit oder ohne abschließenden
Slash werden unter Erhalt der ersten Position dedupliziert.

```sh
build/final/ilic -silent \
  -repositories '/srv/company-models;https://example.org/models' \
  --no-default-repositories \
  -models CompanyModel \
  --ili-version 2.4
```

Jeder Seed enthält ein `ilimodels.xml`; `ilisite.xml` ist optional. Unterstützt
werden absolute und relative lokale Pfade, Windows- und UNC-Pfade,
`file:///tmp/models`, `file:///C:/models`, `file://server/share/models` sowie
HTTP und HTTPS. Percent-Encoding wie `%20` wird in `file://`-Pfaden dekodiert.
Query und Fragment einer HTTP(S)-URI bleiben bei der Normalisierung erhalten.

## Lazy Suchreihenfolge

Eine Modellauflösung baut keinen globalen Katalog auf. Indizes und Site-Dateien
werden erst geladen, wenn die Suche das Repository tatsächlich besucht:

1. Alle Seeds werden in ihrer konfigurierten Reihenfolge nach dem Modell
   durchsucht. Ein Treffer beendet die Suche; spätere Seeds und Site-Links
   bleiben ungeladen.
2. Falls kein Seed trifft, werden die Parent-Links der Seeds breadth-first in
   XML-Reihenfolge durchsucht.
3. Danach folgen Subsidiaries breadth-first. Bei jedem Subsidiary werden dessen
   Parent-Links vor dem nächsten Subsidiary breadth-first abgearbeitet.
4. Normalisierte, bereits besuchte Repository-URIs werden übersprungen.

`listModels()` besucht dagegen bewusst alle erreichbaren Repositories und gibt
alle gültig geparsten `ModelMetadata`-Einträge einschließlich älterer und
`browseOnly`-Versionen zurück. Geladene Indizes, Site-Metadaten und für diese
Manager-Instanz bekannte Ausfälle werden im Speicher wiederverwendet.

Ein nicht erreichbares Repository erzeugt während einer fortsetzbaren Suche
eine Warning `ILIC-REPO-INDEX`. Findet ein späteres Repository das Modell, kann
das Resultat deshalb `success=true` und dennoch Warnings enthalten. Wird das
Modell nirgends gefunden, kommt `ILIC-REPO-NOT-FOUND` als Error hinzu. Ein
fehlendes optionales `ilisite.xml` ist kein Fehler.

## Modell- und Sprachversionen

Versionen werden weder lexikografisch noch als SemVer verglichen. Für die
Kombination aus Modellname und `SchemaLanguage` wird die aktuelle Version über
die in Indexreihenfolge stabile Kette ermittelt:

1. `browseOnly=true` ausfiltern;
2. erste Version ohne `precursorVersion` als Wurzel wählen;
3. jeweils dem ersten Eintrag folgen, dessen `precursorVersion` der aktuellen
   `Version` entspricht;
4. am letzten verbundenen Eintrag stoppen.

Weitere Wurzeln, mehrere Nachfolger, unverbundene Versionen oder eine fehlende
Wurzel erzeugen Warnings. `publishingDate` wird nicht als Ersatzheuristik
verwendet. Damit folgt beispielsweise `2.10 precursorVersion=2.9` auf `2.9`,
ohne die Strings numerisch oder lexikografisch zu vergleichen.

Bei expliziter Sprache wird exakt `ili1`, `ili2_3` oder `ili2_4` verwendet.
Ohne Einschränkung gilt in jedem besuchten Repository die deterministische
Reihenfolge:

1. `ili2_4`
2. `ili2_3`
3. `ili1`

INTERLIS 2.2 wird nicht automatisch gewählt, weil der Compiler diese Sprache
nicht unterstützt.

## Abhängigkeiten und Pfadsicherheit

`dependsOnModel` wird rekursiv in deklarierter Reihenfolge aufgelöst. Das
eingebaute Modell `INTERLIS` benötigt keine Datei. Abhängigkeiten erscheinen im
Resultat vor dem anfordernden Modell. Ein Zyklus meldet den vollständigen Pfad,
beispielsweise `A -> B -> C -> A`, als `ILIC-REPO-CYCLE`.

Mehrere Modelle dürfen dieselbe `.ili`-Datei referenzieren. Sie wird anhand der
normalisierten vollständigen URI nur einmal materialisiert, während alle
Modellnamen als aufgelöst gelten.

Repository-Dateipfade dürfen nur relativ sein. Absolute Pfade, andere URIs,
Laufwerks- und UNC-Pfade sowie `..`-Segmente werden als `ILIC-REPO-PATH`
abgelehnt. Bei lokalen Roots wird zusätzlich komponentenweise geprüft, dass
der normalisierte Zielpfad tatsächlich unterhalb des Roots liegt.

## Cache, Offline-Modus und MD5

Der native Cachepfad wird in dieser Reihenfolge bestimmt:

1. `ILIC_CACHE`;
2. `ILI_CACHE/ilic-v1`;
3. `$HOME/.ilicache/ilic-v1`, unter Windows ersatzweise `USERPROFILE`;
4. System-Tempverzeichnis unter `ilic-cache-v1`.

Metadaten sind standardmäßig 24 Stunden und Modelle sieben Tage frisch. Jeder
Schreibvorgang verwendet eine eindeutige temporäre Datei mit Prozess-ID,
Zufallsanteil und Sequenznummer. Erst nach erfolgreichem Schreiben, Flush und
Close wird atomar veröffentlicht. Ist der konfigurierte Cache nicht
beschreibbar, materialisiert ein verwalteter temporärer Store den erfolgreichen
Download, sodass `ResolvedModel.localPath` stets auf eine lesbare Datei zeigt.

Bei `allowStaleOnError=true` kann ein abgelaufener Eintrag nach einem
Transportfehler als `stale=true` mit Warning verwendet werden. Im Offline-Modus
finden keine Downloads statt; nur vorhandene Cacheeinträge sind verfügbar.
Diese Optionen sind derzeit C++-/JavaScript-API-Optionen, keine CLI-Schalter.

Ist in den Metadaten eine MD5-Prüfsumme angegeben, wird sie bei Netz-, frischem
Cache- und stale Cache-Inhalt case-insensitiv geprüft. Ein beschädigter
Cacheeintrag wird online invalidiert und genau einmal neu geladen. Offline oder
nach einem erfolglosen Retry entsteht `ILIC-REPO-CHECKSUM` mit URI, erwarteter
und tatsächlicher Prüfsumme. Die Prüfung verwendet die tatsächlichen Bytes der
Modelldatei; Zeilenenden werden nicht vorher normalisiert. Repository-
Modelldateien im Quellbaum müssen deshalb mit stabilen LF-Zeilenenden ausgecheckt
werden. Das Repository erzwingt dieses Format für versionierte `.ili`-Dateien
über [`.gitattributes`](../.gitattributes). `validateChecksums=false` deaktiviert
diese Prüfung in der C++-API.

## C++-API

```cpp
#include "ilic/Repository.h"

ilic::RepositoryOptions options;
options.repositories = {
   "/srv/company-models",
   "https://models.interlis.ch"
};
options.cacheDirectory = "/var/cache/ilic";
options.offline = false;
options.allowStaleOnError = true;
options.followSiteLinks = true;
options.validateChecksums = true;

ilic::RepositoryManager manager(std::move(options));
auto result = manager.resolve("DatasetIdx16", "ili2_3");
for (const auto &diagnostic : result.diagnostics)
   std::cerr << diagnostic.code << ": " << diagnostic.message << '\n';
if (!result.success)
   return 1;
```

`RepositoryManager::defaultRepositories()` liefert die CLI-Defaultliste nur
als Konstante; der Konstruktor wendet sie nicht automatisch an.

## JavaScript und Browser

```js
import { RepositoryManager } from "@ilic/tools";
import { NodeFileCache } from "@ilic/tools/node";

const manager = new RepositoryManager({
  repositories: ["https://models.interlis.ch"],
  cache: new NodeFileCache(".cache/ilic"),
  metadataTtlMs: 24 * 60 * 60 * 1000,
  modelTtlMs: 7 * 24 * 60 * 60 * 1000,
  allowStaleOnError: true,
  followSiteLinks: true
});

const workspace = await manager.resolveModel("DatasetIdx16", "ili2_3");
console.log(workspace.models.map(model => model.metadata.name));
```

`MemoryCache` eignet sich für Tests; `BrowserCache` persistiert in IndexedDB
und benötigt bei fremden Origins passende CORS-Header. Ein eigenes `load(uri)`
kann Authentisierung, Proxy oder anwendungsspezifische Dateisystemzugriffe
bereitstellen. Der `NodeFileCache` publiziert Content und Metadaten über je
eindeutige temporäre Pfade; unvollständige Paare werden als Cache-Miss behandelt.
