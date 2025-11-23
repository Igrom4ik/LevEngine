import argparse
import json

parser = argparse.ArgumentParser(description='Inspect a JSONL dataset file and print summaries')
parser.add_argument('--file', '-f', default=r'D:/deepseek/dataset.jsonl', help='Path to jsonl file')
parser.add_argument('--count', '-n', type=int, default=3, help='How many first records to show')
args = parser.parse_args()

path = args.file
n = args.count

try:
    with open(path, 'r', encoding='utf-8') as f:
        # count lines quickly
        total = 0
        for _ in f:
            total += 1
    print(f'File: {path}')
    print(f'Total records: {total}')

    if total == 0:
        print('File is empty')
        raise SystemExit(0)

    print('\nShowing first', n, 'records:\n')
    with open(path, 'r', encoding='utf-8') as f:
        for i in range(n):
            line = f.readline()
            if not line:
                break
            try:
                obj = json.loads(line)
            except Exception as e:
                print(f'Failed to parse JSON on line {i + 1}:', e)
                print(line[:1000])
                continue
            meta = obj.get('meta', {})
            print('--- RECORD', i + 1, '---')
            print('path_relative:', meta.get('path_relative'))
            print('filename:', meta.get('filename'))
            print('language:', meta.get('language'))
            print('size_bytes:', meta.get('size_bytes'), 'lines:', meta.get('lines'))
            print('top_comment:', (meta.get('top_comment') or '').splitlines()[0] if meta.get('top_comment') else '')
            print('\nExcerpt:\n')
            excerpt = meta.get('excerpt') or ''
            print('\n'.join(excerpt.splitlines()[:20]))
            print('\nSynthetic Q&A preview:')
            messages = obj.get('messages', [])
            # print first few QA pairs
            for m in messages[:8]:
                role = m.get('role')
                content = m.get('content', '')
                print(f'[{role}]', content.replace('\n', ' ')[:300])
            print('\n')
except FileNotFoundError:
    print(f'File not found: {path}')
except Exception as e:
    print('Error:', e)
