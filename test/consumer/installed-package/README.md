Diese Fixture verwendet ein bereitgestelltes `cmake --install`-Ergebnis. Sie
weist nach, dass nachgelagerte C- und C++-Projekte das installierte Paket allein
über `find_package(ilic CONFIG REQUIRED)` sowie die Targets `ilic::capi` und
`ilic::core` konsumieren können.
