# Metamodell-Ownership

`metamodel::MetaModelStore` ist der einzige Owner dynamischer
`MMObject`-Instanzen. Er erzeugt sie mit `make<T>()` und hält sie in
`std::unique_ptr<MMObject>`. `MMObject` besitzt einen virtuellen Destruktor,
damit polymorphe Zerstörung sicher ist.

Zeiger in den Metamodellklassen sind nicht besitzende Graphkanten. Sie dürfen
nur auf Objekte desselben Stores zeigen. `discardUnlinked()` ist ausschließlich
für noch nicht publizierte Objekte vorgesehen; normale Aufräumlogik erfolgt
durch RAII und den Store-Destruktor. `clone(destination)` nimmt den Zielstore
explizit entgegen und registriert alle Unterobjekte dort.

Store-Registrygetter liefern konstante Referenzen. Built-in-INTERLIS,
Universalobjekte und Namensindizes gehören jeweils zum konkreten Lauf. Die
öffentlichen Compilation-, Syntax- und Semantic-Snapshots kopieren Strings,
Ranges und IDs und überleben deshalb die Zerstörung ihres Contexts.
