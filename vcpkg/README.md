# vcpkg-Port von ilic

`vcpkg/ports/ilic` ist die lokale Vorlage für das native Bibliothekspaket. Der
Port exportiert `ilic::core` und `ilic::capi`; CLI und nativer
Repository-Transport sind absichtlich deaktiviert.

```sh
$VCPKG_ROOT/vcpkg install ilic:x64-linux --overlay-ports="$PWD/vcpkg/ports"
```

Die publizierte Katalogquelle ist nicht diese Vorlage, sondern das gemeinsame
Git-Registry im Branch `vcpkg-registry`. Registry, Versionierung,
Binary-Cache-Triplets und Konsumenten sind zentral unter
[`docs/ecosystem.md`](../docs/ecosystem.md#vcpkg-registry-und-binary-cache)
dokumentiert. Der Publikationsablauf steht unter
[`docs/release.md`](../docs/release.md#vcpkg-publikation).
