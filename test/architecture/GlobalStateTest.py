#!/usr/bin/env python3
"""CTest entry point for the P2 source-level global-state guard."""

from pathlib import Path
import subprocess
import sys


if __name__ == "__main__":
    root = Path(__file__).resolve().parents[2]
    result = subprocess.run(
        [sys.executable, str(root / "scripts" / "check-core-global-state.py"), str(root)],
        check=False,
    )
    raise SystemExit(result.returncode)
