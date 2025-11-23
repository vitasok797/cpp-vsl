import os
from pathlib import Path


def format_in_dir(target_dir: Path):
    files = []
    extensions = ['.cpp', '.h']
    for ext in extensions:
        files += target_dir.rglob(f'*{ext}')

    for file in files:
        print(file)
        os.system(f'clang-format -i "{file}"')


format_in_dir(Path('vsl'))
format_in_dir(Path('test'))
