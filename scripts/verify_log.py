import argparse
import json
import os
import re
from pathlib import Path

parser = argparse.ArgumentParser(description='Verify process.log against repo structure and dataset JSONL')
parser.add_argument('--log', '-l', default=r'D:/deepseek/process.log', help='Path to process.log')
parser.add_argument('--dataset', '-d', default=r'D:/deepseek/dataset_stream.jsonl',
                    help='Path to dataset jsonl (stream output)')
parser.add_argument('--repo-root', '-r', default='.', help='Repository root (where paths are relative to)')
args = parser.parse_args()

LOG = args.log
DATASET = args.dataset
REPO_ROOT = Path(args.repo_root).resolve()

processed = []
pattern = re.compile(r"Processed:\s+(.+?)\s+\|\s+size=(\d+)\s+bytes\s+\|\s+lines=(\d+)")

if not os.path.exists(LOG):
    print(f"Log file not found: {LOG}")
    raise SystemExit(1)

with open(LOG, 'r', encoding='utf-8', errors='replace') as f:
    for line in f:
        m = pattern.search(line)
        if m:
            rel = m.group(1).strip()
            size = int(m.group(2))
            lines = int(m.group(3))
            processed.append({'rel': rel, 'size': size, 'lines': lines, 'raw': line.strip()})

print(f"Parsed {len(processed)} processed entries from log ({LOG})")

# detect duplicates
from collections import Counter

rels = [p['rel'] for p in processed]
dup_counts = Counter(rels)
dups = [k for k, v in dup_counts.items() if v > 1]
if dups:
    print(f"Found {len(dups)} duplicated path_relative entries in log:")
    for d in dups:
        print(f"  {d}  (count={dup_counts[d]})")
else:
    print("No duplicates in log")

# Check existence and actual size/lines
missing = []
size_mismatch = []
lines_mismatch = []
for p in processed:
    abs_path = REPO_ROOT.joinpath(p['rel'])
    if not abs_path.exists():
        missing.append(p['rel'])
    else:
        try:
            st = abs_path.stat()
            actual_size = st.st_size
            # count lines
            with open(abs_path, 'r', encoding='utf-8', errors='replace') as fh:
                actual_lines = sum(1 for _ in fh)
            if actual_size != p['size']:
                size_mismatch.append({'rel': p['rel'], 'logged': p['size'], 'actual': actual_size})
            if actual_lines != p['lines']:
                lines_mismatch.append({'rel': p['rel'], 'logged': p['lines'], 'actual': actual_lines})
        except Exception as e:
            print(f"Error checking file {abs_path}: {e}")

print('\nSummary of file checks:')
print(f"  missing files: {len(missing)}")
print(f"  size mismatches: {len(size_mismatch)}")
print(f"  line count mismatches: {len(lines_mismatch)}")

if missing:
    print('\nMissing files (present in log but not in repo):')
    for m in missing:
        print('  ', m)

if size_mismatch:
    print('\nFiles with size mismatch:')
    for s in size_mismatch[:20]:
        print('  ', s['rel'], 'logged=', s['logged'], 'actual=', s['actual'])

if lines_mismatch:
    print('\nFiles with line-count mismatch:')
    for s in lines_mismatch[:20]:
        print('  ', s['rel'], 'logged=', s['logged'], 'actual=', s['actual'])

# Now check dataset
if os.path.exists(DATASET):
    print(f"\nReading dataset: {DATASET}")
    ds_rels = []
    count = 0
    with open(DATASET, 'r', encoding='utf-8', errors='replace') as f:
        for line in f:
            count += 1
            try:
                obj = json.loads(line)
                meta = obj.get('meta', {})
                rel = meta.get('path_relative')
                ds_rels.append(rel)
            except Exception:
                pass
    print(f"  dataset records: {count}")

    ds_set = set(ds_rels)
    log_set = set(rels)
    only_in_log = log_set - ds_set
    only_in_ds = ds_set - log_set

    print(f"  entries only in log (not in dataset): {len(only_in_log)}")
    if only_in_log:
        for x in list(only_in_log)[:20]:
            print('   ', x)
    print(f"  entries only in dataset (not in log): {len(only_in_ds)}")
    if only_in_ds:
        for x in list(only_in_ds)[:20]:
            print('   ', x)
else:
    print(f"Dataset file not found: {DATASET}")

print('\nRepository root used:', REPO_ROOT)
print('Done.')
