# Shared INTERLIS vcpkg registry

This branch contains immutable vcpkg versions for the `ilic` and `iox-cpp`
ports. The current catalogue state is defined by `versions/baseline.json`;
this README deliberately does not duplicate individual version numbers.

Applications must pin a full registry commit as their baseline. Historical
entries remain immutable when the baseline advances. New snapshots use
`X.Y.Z-snapshot.g<12-character-source-SHA>`; packaging-only corrections use
vcpkg `port-version`.

Only the serialized registry workflow in `ilic-fork` writes this branch.
