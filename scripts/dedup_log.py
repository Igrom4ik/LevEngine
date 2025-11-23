import argparse
import os
import re
from pathlib import Path

parser = argparse.ArgumentParser(description='Deduplicate process log by Processed: entries (keep first occurrence)')
parser.add_argument('--input', '-i', default=r'D:/deepseek/process.log', help='Input log file')
parser.add_argument('--output', '-o', default=r'D:/deepseek/process.dedup.log', help='Output deduplicated log file')
parser.add_argument('--repo-root', '-r', default='.', help='Repository root to resolve relative paths')
parser.add_argument('--exclude', '-x', nargs='*',
                    default=['vendor', '.git', 'cmake-build', 'cmake-build-debug', 'build', 'bin', '.venv', 'dist',
                             'out', '__pycache__'], help='Directory fragments to exclude')
args = parser.parse_args()

inp = Path(args.input)
out = Path(args.output)
REPO_ROOT = Path(args.repo_root).resolve()
EXCLUDE = [e.lower() for e in args.exclude]

if not inp.exists():
    print(f'Input log not found: {inp}')
    raise SystemExit(1)

seen = set()
lines_out = []
skipped = 0
kept = 0
# regex to find Processed: <path> (path may contain spaces) until first '|' or end
proc_re = re.compile(r"Processed:\s*(?P<path>[^|\n\r]+)")
with inp.open('r', encoding='utf-8', errors='replace') as f:
    for line in f:
        m = proc_re.search(line)
        if m:
            raw_path = m.group('path').strip()
            # normalize slashes and remove surrounding quotes if any
            norm = raw_path.strip(' \"\'')
            norm = norm.replace('/', os.path.sep).replace('\\', os.path.sep)
            # If path is absolute, make it relative to repo if possible
            candidate = Path(norm)
            try:
                if candidate.is_absolute():
                    rel = str(candidate.relative_to(REPO_ROOT))
                else:
                    rel = norm
            except Exception:
                rel = norm
            rel_low = rel.lower()
            # skip if in excluded dirs
            skip = False
            for ex in EXCLUDE:
                # match fragment in path parts
                if ex in rel_low:
                    skip = True
                    break
            if skip:
                skipped += 1
                continue
            # dedupe by the relative path (proc key)
            proc_key = rel.replace('\\', '/').strip()
            if proc_key in seen:
                # already seen
                continue
            seen.add(proc_key)
            lines_out.append(line)
            kept += 1
        else:
            # keep other lines (e.g., Appended to output) as-is
            lines_out.append(line)

out.parent.mkdir(parents=True, exist_ok=True)
with out.open('w', encoding='utf-8') as f:
    f.writelines(lines_out)

print(f'Wrote deduplicated log to: {out} (entries kept: {kept}, skipped excluded: {skipped})')
