"""Проверка покрытия технологической информации в JSONL датасете.
Запуск (в PowerShell из корня репозитория):
$env:PYTHONIOENCODING='utf-8'; $env:PYTHONUTF8='1'
python scripts/check_tech_coverage.py --dataset "D:/deepseek/myAi/pointjs_docs.jsonl"

Скрипт найдёт файлы в Engine/Source, где встречаются сигнатуры OpenGL/Vulkan/DirectX и сравнит их с записями в dataset JSONL.
Выведет список файлов, для которых найдены признаки рендер-бэкенда, но в dataset нет соответствующей записи technologies/render_primary.
"""
import argparse
import json
import re
from pathlib import Path

ENGINE_SRC = Path('Engine/Source')
DEFAULT_DATASET = Path('D:/deepseek/myAi/pointjs_docs.jsonl')

GL_PAT = re.compile(
    r"\b(glEnable|glCreateShader|glShaderSource|glCompileShader|glUseProgram|glUniform\w*|glBindBuffer|glVertexAttribPointer|glDrawElements|glDrawArrays|GL_\w+|#include\s*<GL/)")
VK_PAT = re.compile(r"\b(vkCreateInstance|vkCreateDevice|VK_)")
DX_PAT = re.compile(r"\b(D3D12CreateDevice|D3D11CreateDevice|ID3D12Device|ID3D11Device|#include\s*<d3d)", re.IGNORECASE)


def scan_files_for_backends(root: Path):
    files = {}
    for p in root.rglob('*'):
        if p.is_file() and p.suffix.lower() in ('.cpp', '.c', '.h', '.hpp', '.inl'):
            s = p.read_text(encoding='utf-8', errors='replace')
            gl = bool(GL_PAT.search(s))
            vk = bool(VK_PAT.search(s))
            dx = bool(DX_PAT.search(s))
            if gl or vk or dx:
                files[str(p).replace('\\', '/')] = {'gl': gl, 'vk': vk, 'dx': dx}
    return files


def load_dataset_index(dataset_path: Path):
    idx = {}
    if not dataset_path.exists():
        print('Dataset not found:', dataset_path)
        return idx
    with dataset_path.open('r', encoding='utf-8') as fh:
        for i, line in enumerate(fh, 1):
            line = line.strip()
            if not line: continue
            try:
                j = json.loads(line)
            except Exception as e:
                print('JSON parse error line', i, e)
                continue
            meta = j.get('metadata') or j.get('meta') or {}
            path_rel = meta.get('path_relative') or meta.get('path') or None
            if path_rel:
                idx[path_rel.replace('\\', '/')] = meta
    return idx


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--dataset', '-d', default=str(DEFAULT_DATASET))
    args = ap.parse_args()

    dataset_p = Path(args.dataset)
    print('Scanning source for GL/VK/DX patterns in', ENGINE_SRC)
    files = scan_files_for_backends(ENGINE_SRC)
    print(f'Found {len(files)} files with backend-like patterns')

    print('Loading dataset index from', dataset_p)
    ds = load_dataset_index(dataset_p)
    print('Dataset entries loaded:', len(ds))

    missing = []
    matched = []
    for fpath, info in files.items():
        # make relative path from repo root
        try:
            rel = str(Path(fpath).relative_to(Path.cwd())).replace('\\', '/')
        except Exception:
            rel = fpath
        meta = ds.get(rel) or ds.get(rel.replace('Engine/', 'Engine/')) or ds.get(rel.split('Engine/')[-1])
        has_meta_tech = False
        if meta:
            tech = meta.get('technologies') or {}
            rp = meta.get('render_primary')
            if (tech and any(tech.get(k) for k in ('render_backends', 'audio', 'physics', 'network'))) or rp:
                has_meta_tech = True
        if not has_meta_tech:
            missing.append((rel, info))
        else:
            matched.append((rel, info))

    print('\nFiles with backend-like code but missing tech metadata in dataset:')
    for rel, info in missing:
        flags = ','.join(k for k, v in info.items() if v)
        print(f' - {rel} -> patterns: {flags}')

    print('\nFiles with backend metadata present:')
    for rel, info in matched[:50]:
        flags = ','.join(k for k, v in info.items() if v)
        print(f' - {rel} -> patterns: {flags}')

    print(f'\nTotal files found: {len(files)}; missing metadata: {len(missing)}; matched: {len(matched)}')


if __name__ == '__main__':
    main()
