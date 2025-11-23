#!/usr/bin/env python3
"""
Check dataset coverage of repository source files.
Outputs lists of files missing from dataset, files in dataset but missing on disk, and per-directory coverage.
"""
import argparse
import json
import os
from collections import defaultdict
from pathlib import Path

parser = argparse.ArgumentParser(description='Compare repo source files to dataset JSONL and report coverage')
parser.add_argument('--repo-root', '-r', default='.', help='Repository root to scan')
parser.add_argument('--dataset', '-d', default='D:/deepseek/test_dataset.jsonl', help='Dataset jsonl file')
parser.add_argument('--exts', default='.c,.cpp,.cc,.cxx,.h,.hpp,.py,.md,.txt',
                    help='Comma-separated extensions to include')
parser.add_argument('--exclude', default='.git,cmake-build-debug,build,bin,.venv',
                    help='Comma-separated dir names to exclude')
parser.add_argument('--top-n', type=int, default=20, help='How many missing files to show')
args = parser.parse_args()

REPO_ROOT = Path(args.repo_root).resolve()
DATASET_PATH = Path(args.dataset)
EXTS = tuple(x.strip().lower() for x in args.exts.split(','))
EXCLUDE = set(x.strip().lower() for x in args.exclude.split(','))
TOP_N = args.top_n

if not DATASET_PATH.exists():
    print('Dataset file not found:', DATASET_PATH)
    raise SystemExit(2)

# collect repo files
repo_files = []
for root, dirs, files in os.walk(REPO_ROOT):
    # prune excluded dirs
    dirs[:] = [d for d in dirs if d.lower() not in EXCLUDE]
    for fn in files:
        if not any(fn.lower().endswith(ext) for ext in EXTS):
            continue
        p = Path(root) / fn
        try:
            rel = str(p.relative_to(REPO_ROOT)).replace('\\', '/')
        except Exception:
            rel = os.path.relpath(str(p), start=str(REPO_ROOT)).replace('\\', '/')
        repo_files.append(rel)

repo_set = set(repo_files)

# read dataset meta.path_relative values
ds_paths = []
with DATASET_PATH.open('r', encoding='utf-8', errors='replace') as f:
    for i, line in enumerate(f, start=1):
        line = line.strip()
        if not line:
            continue
        try:
            obj = json.loads(line)
        except Exception:
            continue
        meta = obj.get('meta') or {}
        rel = meta.get('path_relative') or meta.get('path')
        if not rel:
            continue
        # normalize
        rel = rel.replace('\\', '/')
        # if absolute path, try to relativize
        try:
            p = Path(rel)
            if p.is_absolute():
                rel = str(p.relative_to(REPO_ROOT)).replace('\\', '/')
        except Exception:
            pass
        ds_paths.append(rel)

ds_set = set(ds_paths)

only_in_repo = sorted(repo_set - ds_set)
only_in_dataset = sorted(ds_set - repo_set)

print('Repository root:', REPO_ROOT)
print('Dataset:', DATASET_PATH)
print('Total source files (matching exts):', len(repo_set))
print('Total records in dataset (with meta.path_relative):', len(ds_set))
print('\nFiles present in repo but MISSING in dataset:', len(only_in_repo))
for p in only_in_repo[:TOP_N]:
    print('  ', p)
if len(only_in_repo) > TOP_N:
    print('   ... and', len(only_in_repo) - TOP_N, 'more')

print('\nFiles present in dataset but NOT on disk:', len(only_in_dataset))
for p in only_in_dataset[:TOP_N]:
    print('  ', p)
if len(only_in_dataset) > TOP_N:
    print('   ... and', len(only_in_dataset) - TOP_N, 'more')

# per-directory coverage (top-level folder)
dir_counts = defaultdict(lambda: {'repo': 0, 'ds': 0})
for rel in repo_set:
    top = rel.split('/', 1)[0] if '/' in rel else rel
    dir_counts[top]['repo'] += 1
for rel in ds_set:
    top = rel.split('/', 1)[0] if '/' in rel else rel
    dir_counts[top]['ds'] += 1

print('\nPer-directory coverage (top-level):')
rows = []
for k, v in dir_counts.items():
    repo_c = v['repo']
    ds_c = v['ds']
    pct = (ds_c / repo_c * 100) if repo_c else 0
    rows.append((k, repo_c, ds_c, pct))
rows.sort(key=lambda x: -x[2])
for k, repo_c, ds_c, pct in rows:
    print(f"  {k:30} repo={repo_c:5} dataset={ds_c:5} coverage={pct:5.1f}%")

# show few examples of missing files with small preview
if only_in_repo:
    print('\nSample missing files content (first 200 chars):')
    for p in only_in_repo[:min(10, len(only_in_repo))]:
        fp = REPO_ROOT.joinpath(p)
        try:
            txt = fp.read_text(encoding='utf-8', errors='replace')[:200]
        except Exception as e:
            txt = f'ERROR reading: {e}'
        print('---', p)
        print(txt)

print('\nDone.')
