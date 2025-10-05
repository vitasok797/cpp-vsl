import os
from pathlib import Path

work_dir = Path('vsl')

files = []
extensions = ['.cpp', '.h']
for ext in extensions:
    files += work_dir.rglob(f'*{ext}')

for file in files:
    print(file)
    os.system(f'clang-format -i "{file}"')
