#!/usr/bin/env python3
#
# Board-local wrapper for esp_board_manager code generation.

from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path


BOARD_NAME = "lilygo_t_display_p4"
BOARD_DIR = Path(__file__).resolve().parent
PROJECT_DIR = BOARD_DIR.parents[2]
ORIGINAL_GENERATOR = PROJECT_DIR / "managed_components" / "espressif__esp_board_manager" / "gen_bmgr_config_codes.py"
GENERATED_KCONFIG = PROJECT_DIR / "components" / "gen_bmgr_codes" / "Kconfig.projbuild"
BOARD_KCONFIG = BOARD_DIR / "Kconfig.projbuild"

def run_original_generator() -> int:
    args = sys.argv[1:]
    if not args:
        args = ["-b", BOARD_NAME, "-c", str(PROJECT_DIR / "boards"), "--project-dir", str(PROJECT_DIR)]

    env = os.environ.copy()
    env.setdefault("PYTHONIOENCODING", "utf-8")
    return subprocess.call([sys.executable, str(ORIGINAL_GENERATOR), *args], cwd=str(PROJECT_DIR), env=env)


def append_board_kconfig() -> None:
    if not BOARD_KCONFIG.exists() or not GENERATED_KCONFIG.exists():
        return

    generated = GENERATED_KCONFIG.read_text(encoding="utf-8").rstrip()
    board_kconfig = BOARD_KCONFIG.read_text(encoding="utf-8").strip()
    if not board_kconfig:
        return

    GENERATED_KCONFIG.write_text(
        f"{generated}\n\n# Board-specific Kconfig.projbuild\n{board_kconfig}\n",
        encoding="utf-8",
    )


def main() -> int:
    ret = run_original_generator()
    if ret != 0:
        return ret

    append_board_kconfig()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
