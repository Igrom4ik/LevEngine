import argparse
import hashlib
import json
import os
import re
import sys
from pathlib import Path

# Небольшой, надёжный скрипт для создания JSONL из исходников

parser = argparse.ArgumentParser(description='Collect source files into jsonl dataset')
parser.add_argument('--root', '-r',
                    default=os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'Engine', 'Source')),
                    help='Root folder to scan')
parser.add_argument('--repo-root', default=os.path.abspath(os.path.join(os.path.dirname(__file__), '..')),
                    help='Repository root to compute path_relative (default: parent of scripts/)')
parser.add_argument('--out', '-o', default=os.path.abspath(os.path.join('D:/deepseek', 'dataset.jsonl')),
                    help='Output jsonl file')
parser.add_argument('--ext', '-e', nargs='+', default=['.h', '.hpp', '.cpp', '.c', '.cc', '.py', '.md'],
                    help='File extensions to include')
parser.add_argument('--exclude', '-x', nargs='*', default=['.git', 'cmake-build-debug', 'build', 'bin', '.venv'],
                    help='Directory names to exclude')
parser.add_argument('--excerpt-lines', type=int, default=50)
parser.add_argument('--stream', action='store_true', help='Append entries incrementally')
parser.add_argument('--verbose', action='store_true')
args = parser.parse_args()

ROOT = Path(args.root).resolve()
REPO = Path(args.repo_root).resolve()
OUT = Path(args.out).resolve()
EXTS = tuple(e.lower() for e in args.ext)
EXCLUDE = set(e.lower() for e in args.exclude)
EXCERPT_LINES = args.excerpt_lines
STREAM = args.stream
VERBOSE = args.verbose

BINARY_EXTS = {'.exe', '.dll', '.lib', '.pdb', '.ilk', '.o', '.obj', '.a', '.so', '.class', '.jar', '.pyc', '.pyo'}


def is_binary_by_ext(p: Path):
    return p.suffix.lower() in BINARY_EXTS


def sha256_text(text: str) -> str:
    h = hashlib.sha256()
    h.update(text.encode('utf-8'))
    return h.hexdigest()


def top_comment(code: str) -> str:
    # return leading // comments or first /* */ block
    lines = code.splitlines()
    top = []
    for ln in lines[:20]:
        s = ln.strip()
        if s.startswith('//'):
            top.append(s.lstrip('/').strip())
        elif s == '':
            if top:
                break
        else:
            break
    if top:
        return '\n'.join(top)
    m = re.search(r"/\*([\s\S]*?)\*/", '\n'.join(lines[:200]))
    return (m.group(1).strip() if m else '')


def make_entry(path: Path):
    try:
        raw = path.read_bytes()
    except Exception as e:
        if VERBOSE:
            print('read error', path, e, file=sys.stderr)
        return None
    try:
        text = raw.decode('utf-8', errors='replace')
    except Exception:
        text = ''

    lines = text.splitlines()
    excerpt = '\n'.join(lines[:EXCERPT_LINES])
    meta = {
        'path': str(path),
        'path_relative': str(path.relative_to(REPO)) if REPO in path.parents or path == REPO else os.path.relpath(
            str(path), start=str(REPO)).replace('\\', '/'),
        'filename': path.name,
        'size_bytes': len(raw),
        'lines': len(lines),
        'mtime': path.stat().st_mtime,
        'language': path.suffix.lower().lstrip('.'),
        'excerpt': excerpt,
        'top_comment': top_comment(text),
        'sha256': sha256_text(text)
    }

    messages = [
        {'role': 'system', 'content': 'You are a helpful assistant that answers concisely in Russian.'},
        {'role': 'system', 'content': (excerpt if len(excerpt) < 2000 else excerpt[:2000])}
    ]

    # simple synthetic QA
    summary = meta['top_comment'] or (excerpt.splitlines()[0] if excerpt.splitlines() else '')
    messages.append({'role': 'user', 'content': 'Что делает этот файл?'})
    messages.append({'role': 'assistant', 'content': summary})

    return {'messages': messages, 'meta': meta}


def walk_and_collect(root: Path):
    entries = []
    for dirpath, dirnames, filenames in os.walk(root):
        # prune excluded directories
        dirnames[:] = [d for d in dirnames if d.lower() not in EXCLUDE]
        for fn in filenames:
            p = Path(dirpath) / fn
            if is_binary_by_ext(p):
                continue
            if not fn.lower().endswith(EXTS):
                continue
            entry = make_entry(p)
            if entry:
                entries.append(entry)
    return entries


def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    if STREAM:
        fh = OUT.open('a', encoding='utf-8')
        count = 0
        for entry in walk_and_collect(ROOT):
            fh.write(json.dumps(entry, ensure_ascii=False) + '\n')
            count += 1
            if VERBOSE:
                print('written', entry['meta']['path_relative'])
        fh.close()
        print('wrote', count, 'entries to', OUT)
    else:
        entries = walk_and_collect(ROOT)
        with OUT.open('w', encoding='utf-8') as fh:
            for e in entries:
                fh.write(json.dumps(e, ensure_ascii=False) + '\n')
        print('wrote', len(entries), 'entries to', OUT)


if __name__ == '__main__':
    main()
