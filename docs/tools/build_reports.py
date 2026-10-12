"""Build both final editions with XeLaTeX/BibTeX (sources are UTF-8)."""
from pathlib import Path
import subprocess
import shutil
import re
import urllib.request
ROOT = Path(__file__).resolve().parents[1]
for name in ('xelatex', 'bibtex'):
    if not shutil.which(name):
        raise SystemExit(f'{name} is not on PATH; install a TeX distribution.')
fonts = ROOT / 'fonts'
fonts.mkdir(exist_ok=True)
# Official upstream, pinned release; downloaded fonts remain untracked.
for weight in ('Regular', 'Bold'):
    target = fonts / f'Vazirmatn-{weight}.ttf'
    if not target.exists():
        url = f'https://raw.githubusercontent.com/rastikerdar/vazirmatn/v33.003/fonts/ttf/Vazirmatn-{weight}.ttf'
        urllib.request.urlretrieve(url, target)
for language in ('english', 'persian'):
    directory = ROOT / language
    commands = [
        ['xelatex', '-interaction=nonstopmode', '-halt-on-error', 'main_report.tex'],
        ['bibtex', 'main_report'],
        ['xelatex', '-interaction=nonstopmode', '-halt-on-error', 'main_report.tex'],
        ['xelatex', '-interaction=nonstopmode', '-halt-on-error', 'main_report.tex'],
    ]
    with (directory / 'build-console.txt').open('w', encoding='utf-8') as log:
        for command in commands:
            result = subprocess.run(command, cwd=directory, stdout=log, stderr=subprocess.STDOUT)
            if result.returncode:
                raise SystemExit(f'{language}: build failed; see {log.name}')
    final_log = (directory / 'main_report.log').read_text(encoding='utf-8', errors='replace')
    for pattern in (r'Overfull \\[hv]box', 'Missing character:', 'undefined references', 'Citation .* undefined'):
        if re.search(pattern, final_log):
            raise SystemExit(f'{language}: unresolved layout/reference issue: {pattern}')
    print(f'Built {language}/main_report.pdf')
