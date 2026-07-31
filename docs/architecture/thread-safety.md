# Thread-Safety-Vertrag

| Operation | gleiche Session | verschiedene Sessions |
| --- | --- | --- |
| `putSource` / `removeSource` | serialisiert | parallel |
| `parse` | serialisiert | parallel |
| `compile` | serialisiert | parallel |
| `analyze` / `compileAndAnalyze` | serialisiert | parallel |
| direkter mutabler `sources()`-Zugriff | externe Synchronisation | unabhängig |
| C-ABI verschiedene Handles | parallel | parallel |

Die Serialisierung erfolgt ausschließlich mit dem Mutex der jeweiligen
`CompilerSession`. Es gibt keinen globalen Compiler-Mutex. Jeder Lauf erzeugt
seinen eigenen Context, Logger, Katalog und Metamodellstore. Die C-ABI-Registry
schützt nur Handles und Resultate mit ihrem kurzen Registry-Mutex; der eigentliche
Compileraufruf läuft nach Freigabe dieses Locks.

Gleiche Ausgabedateien dürfen nicht gleichzeitig beschrieben werden. Das ist ein
Aufruferfehler; unabhängige Generatorinstanzen und Zielpfade sind parallel
verwendbar.
