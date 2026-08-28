import importlib.util
import pathlib
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "release_metadata", ROOT / "scripts" / "release_metadata.py"
)
release_metadata = importlib.util.module_from_spec(SPEC)
assert SPEC.loader
SPEC.loader.exec_module(release_metadata)


class ReleaseMetadataTest(unittest.TestCase):
    def setUp(self):
        self.sha = "0123456789abcdef0123456789abcdef01234567"

    def test_snapshot_is_deterministic(self):
        self.assertEqual(
            release_metadata.snapshot_version("0.10.0", self.sha),
            "0.10.0-snapshot.g0123456789ab",
        )

    def test_snapshot_requires_matching_sha(self):
        with self.assertRaisesRegex(ValueError, "does not match"):
            release_metadata.validate_artifact_version(
                "0.10.0-snapshot.gaaaaaaaaaaaa", self.sha
            )

    def test_old_snapshot_formats_are_rejected_for_new_artifacts(self):
        for version in (
            "0.10.0-SNAPSHOT.20260826043335.32930660314",
            "0.10.0-snapshot.01234567",
        ):
            with self.subTest(version=version):
                with self.assertRaises(ValueError):
                    release_metadata.validate_artifact_version(version, self.sha)

    def test_manifest_keeps_build_metadata_out_of_version(self):
        manifest = release_metadata.release_manifest(
            project_root=ROOT,
            artifact_version="0.10.0-snapshot.g0123456789ab",
            source_sha=self.sha,
            run_id="987654321",
            published_at="2026-08-28T18:00:00Z",
            toolchain="emscripten-4.0.10",
        )
        self.assertEqual(manifest["sourceSha"], self.sha)
        self.assertEqual(manifest["build"]["githubRunId"], "987654321")
        self.assertNotIn("987654321", manifest["artifactVersion"])


if __name__ == "__main__":
    unittest.main()
