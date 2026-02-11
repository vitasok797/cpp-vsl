from __future__ import annotations

import os
from pathlib import Path

PROJECT_SRC_DIR = 'vsl'
CLANG_FORMAT_CONFIG_PATH = None


def format_in_dir(target_dir: str | Path | None) -> None:
    if not target_dir:
        return

    target_dir = Path(target_dir)
    if not target_dir.is_dir():
        return

    files = []
    extensions = ['.cpp', '.h']
    for ext in extensions:
        files += target_dir.rglob(f'*{ext}')

    for file in sorted(files):
        print(file)
        command = 'clang-format'
        if CLANG_FORMAT_CONFIG_PATH:
            command += f' --style=file:"{CLANG_FORMAT_CONFIG_PATH}"'
        command += f' -i "{file}"'
        os.system(command)


def main() -> None:
    format_in_dir(PROJECT_SRC_DIR)
    format_in_dir('example')
    format_in_dir('test')


if __name__ == '__main__':
    main()
