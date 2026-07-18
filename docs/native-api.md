# Native APIs und JSON-Protokoll

[Dokumentationsindex](README.md) · [Diagnostik](diagnostik-und-logging.md) · [WASM](wasm.md)

Die native Bibliothek hat drei C++-Oberflächen und eine stabile C-Grenze:

- `ilic::CompilerSession` kompiliert virtuelle oder native Quellen;
- `ilic::Formatter` formatiert INTERLIS 2.3/2.4;
- `ilic::RepositoryManager` beschafft Modell-Workspaces;
- `ilic/capi.h` transportiert Compile- und Format-Requests als JSON v1.

## C++: virtuelle Compiler-Session

```cpp
#include "ilic/Compiler.h"

ilic::CompilerSession session;
session.putSource("memory:///Base.ili",baseSource,1);
session.putSource("memory:///Example.ili",exampleSource,2);

ilic::CompilationRequest request;
request.roots = {"memory:///Example.ili"};
request.options.autoSearch = true;
request.options.warningsAsErrors = false;

ilic::CompilationResult result = session.compile(request);
if (!result.success) {
   for (const auto &diagnostic : result.diagnostics)
      std::cerr << diagnostic.code << ": " << diagnostic.message << '\n';
}
```

`putSource` kopiert URI und UTF-8-Inhalt. Derselbe URI kann mit einem neuen
Dokumentstand ersetzt werden; `removeSource` entfernt ihn. Die Versionsnummer
wird gespeichert, beeinflusst aber die Semantik nicht und ist für editornahe
Hosts vorgesehen.

Für einen bekannten UTF-8-Byteoffset liefert
`session.sources().position(uri,byteOffset)` Zeile und UTF-16-Spalte. Diagnostics
enthalten diesen Byteoffset derzeit noch nicht in jedem Fehlerpfad; siehe
[Positionen und Unicode](diagnostik-und-logging.md#positionen-und-unicode).

Beim Auflösen von Imports durchsucht eine Session zuerst alle registrierten
virtuellen Quellen. So kann ein WASM- oder LSP-Host einen vollständigen
Workspace ohne Dateisystem bereitstellen.

Das vollständige, kompilierte Beispiel ist
[`examples/cpp-session.cpp`](examples/cpp-session.cpp).

### Request-Optionen

| Feld | Standard | Funktion |
| --- | --- | --- |
| `roots` | – | URIs der zu kompilierenden Wurzeldateien |
| `options.autoSearch` | `true` | automatische Importauflösung |
| `options.warningsAsErrors` | `false` | Warnungen zusätzlich als Fehler zählen |
| `options.modelDirectories` | leer | native Verzeichnisse für Modell-Suche |
| `externalMetaAttributes` | leer | externe Metaattribute für Elemente |

Der gegenwärtig semantisch ausgewertete externe Attributname ist
`ili2c.translationOf` auf einem Modell:

```cpp
request.externalMetaAttributes.push_back({
   "TranslatedModel", "ili2c.translationOf", "BaseModel"
});
```

### Compilation Result

```cpp
struct CompilationResult {
   bool success;
   bool cancelled;
   int errorCount;
   int warningCount;
   std::vector<std::string> missingModels;
   std::vector<CompiledModel> models;
   std::vector<Diagnostic> diagnostics;
   std::vector<LogEvent> logs;
};
```

`cancelled` ist für den stabilen Vertrag reserviert; eine aktive Cancel-API ist
noch nicht implementiert. Kompilationen verschiedener Sessions werden im
aktuellen Prozess serialisiert. Source-Verwaltung und Formatierung können
weiterhin pro Session beziehungsweise pro Aufruf organisiert werden.

## C++: Formatter

```cpp
#include "ilic/Formatter.h"

ilic::FormatOptions options;
options.indentSize = 2;
options.requireValidSyntax = true;
ilic::FormatResult result = ilic::Formatter().format(uri,source,options);
```

Siehe [Formatter](formatter.md) für den vollständigen Vertrag.

## C++: RepositoryManager

```cpp
#include "ilic/Repository.h"

ilic::RepositoryOptions options;
options.repositories = {"https://models.interlis.ch"};
options.offline = false;

ilic::RepositoryManager manager(options);
ilic::RepositoryResult workspace = manager.resolve("DatasetIdx16","ili2_3");
```

Der RepositoryManager ist absichtlich eine eigene Library. Der Compiler-Core
bleibt für einen gegebenen Satz Source-Buffers deterministisch; das native CLI
linkt beide Komponenten. Details: [Repositories](repositories.md).

## C-ABI v1

Der öffentliche Header ist [`../include/ilic/capi.h`](../include/ilic/capi.h).
Er ist C11-kompatibel, obwohl die statische Implementierung intern C++ nutzt.

```c
#include "ilic/capi.h"

uint32_t session = ilic_session_create();
ilic_session_put_source(session,
   uri, strlen(uri),
   (const uint8_t *)source, strlen(source),
   1);

uint32_t result = ilic_compile(session,request_json,strlen(request_json));
size_t length = 0;
const char *json = ilic_result_json(result,&length);
fwrite(json,1,length,stdout);

ilic_result_destroy(result);
ilic_session_destroy(session);
```

Das vollständige Beispiel ist [`examples/c-api.c`](examples/c-api.c).

### Ownership

1. `ilic_session_create` erzeugt einen Session-Handle.
2. `ilic_session_put_source` kopiert URI und Source; die Caller-Buffer dürfen
   unmittelbar danach freigegeben werden.
3. `ilic_compile` oder `ilic_format` erzeugt einen Result-Handle.
4. `ilic_result_json` liefert einen nicht nullterminierungsabhängigen Pointer
   plus Länge. Er bleibt bis `ilic_result_destroy` gültig.
5. Jeder Result- und Session-Handle muss genau einmal zerstört werden.
6. Speicher aus `ilic_alloc` wird ausschließlich mit `ilic_free` freigegeben.

`ilic_abi_version()` liefert derzeit `1`. ABI-Version und JSON-`schemaVersion`
werden unabhängig voneinander versioniert.

## Compilation Request JSON v1

```json
{
  "schemaVersion": 1,
  "roots": ["memory:///Example.ili"],
  "options": {
    "autoSearch": true,
    "warningsAsErrors": false,
    "modelDirectories": []
  },
  "externalMetaAttributes": [
    {
      "element": "TranslatedModel",
      "name": "ili2c.translationOf",
      "value": "BaseModel"
    }
  ]
}
```

Schema:
[`compilation-request-v1.schema.json`](../schemas/compilation-request-v1.schema.json).
`roots` muss mindestens einen nicht leeren URI enthalten. Unbekannte
Schema-Versionen und falsche Typen ergeben eine Diagnostic mit
`ILIC-ABI-REQUEST` statt eines Prozessabbruchs.

## Compilation Result JSON v1

```json
{
  "schemaVersion": 1,
  "abiVersion": 1,
  "compilerVersion": "0.9.9",
  "kind": "compilation",
  "success": true,
  "cancelled": false,
  "errorCount": 0,
  "warningCount": 0,
  "missingModels": [],
  "models": [
    {
      "name": "Example",
      "iliVersion": "2.3",
      "uri": "memory:///Example.ili",
      "metaAttributes": [
        { "name": "displayName", "value": "Documentation example" }
      ]
    }
  ],
  "diagnostics": [],
  "logs": []
}
```

Schema:
[`compilation-result-v1.schema.json`](../schemas/compilation-result-v1.schema.json).
Die Reihenfolge von `models` kann das eingebaute Modell `INTERLIS` und
Abhängigkeiten vor dem Root-Modell enthalten. Konsumenten sollen Modelle nach
`name` oder `uri` suchen, nicht eine feste Arrayposition voraussetzen.

## Format JSON v1

```json
{
  "schemaVersion": 1,
  "uri": "memory:///Example.ili",
  "options": {
    "indentSize": 2,
    "requireValidSyntax": true
  }
}
```

Die Quelle muss vorher in derselben Session registriert sein. Die Schemas sind:

- [`format-request-v1.schema.json`](../schemas/format-request-v1.schema.json)
- [`format-result-v1.schema.json`](../schemas/format-result-v1.schema.json)

## Linken

Mit CMake:

```cmake
add_executable(my_compiler main.cpp)
target_link_libraries(my_compiler PRIVATE ilic::core)

add_executable(my_c_host main.c)
set_target_properties(my_c_host PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_c_host PRIVATE ilic::capi)
```

Für Repository-Zugriff wird zusätzlich `ilic::repository` gelinkt. Der
Repository-Target bringt libcurl und libxml2 transitiv mit.
