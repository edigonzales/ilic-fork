# P5: Inkrementelle Compiler-Sessions

Eine `ilic::CompilerSession` hält Quellen, Parser-Artefakte und semantische
Root-Ergebnisse über mehrere Anfragen hinweg. Quellen werden über
`updateSource(uri, text, version)` oder `removeSource(uri)` synchronisiert;
`SourceUpdateResult` unterscheidet insbesondere unveränderte Bytes,
Versionsänderungen, Inhaltsänderungen und Reintroduktionen. Eine kleinere
Dokumentversion ändert keine bereits akzeptierte Quelle.

## Parser-Artefakt

`ParsedSourceArtifact` besitzt die ANTLR-Eingabe, den Lexer, den Tokenstrom und
den Parser für genau eine unveränderliche Inhaltsrevision. Syntaxprojektion,
Editorprojektion, Headerdaten und der Compiler-Visitor verwenden dieses
Artefakt. Der sitzungsgebundene `ParsedSourceCache` verwendet URI, Hash,
Byte-Länge und einen Byte-/Eintrags-LRU; seine Grenzen sind über
`IncrementalCacheOptions` konfigurierbar.

## Semantik

`compileAndAnalyze` bildet den Root-Schlüssel aus Anfrageoptionen,
Compiler-/ABI-/Grammatur-Fingerprint und der transitive Closure mit ihren
Source-Identitäten. Ein Treffer aktualisiert nur die sichtbaren
Dokumentversionen. Inhaltsänderungen und Entfernen/Reintroduzieren invalidieren
betroffene Root-Einträge; unabhängige Roots bleiben wiederverwendbar. Der
`RootAnalysisCache` hat eigene LRU-Einträge und Speichergrenzen.

## Language Service und Web IDE

Save-getriggerte Kompilierung bleibt der semantische Pfad. Repository-Modelle
werden als neue Quellen in die bestehende Session aufgenommen und lösen eine
erneute Analyse ohne normalen Worker-Restart oder Replay aus. Der Compiler-
Worker exponiert die Statistik- und Cache-Clear-Operationen additiv.

Die Web IDE verwendet `WorkspaceSourceSynchronizer`: Bei Workspace-Refreshes
werden nur hinzugekommene, geänderte oder entfernte Dateien an den
Language Service weitergereicht. Gleiche Bytes werden nicht erneut gesendet;
explizite Versionsänderungen bleiben als Version-only-Update sichtbar.

## Beobachtbarkeit

`IncrementalStats` und `IncrementalTrace` sind über C++ sowie additiv über C
ABI/WASM verfügbar. Die Statistik enthält Source-Klassifikation,
Parser-Hits/-Misses, Evictions, Root-Hits/-Misses und Invalidationen. Caches
können über `clearIncrementalCaches()` geleert werden, ohne die Quellen der
Session zu verlieren.
