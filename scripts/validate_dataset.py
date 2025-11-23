import json
import sys
from pathlib import Path

REQUIRED_META = ['path', 'path_relative', 'filename', 'size_bytes', 'lines', 'mtime', 'language', 'excerpt',
                 'top_comment', 'sha256']

p = Path(sys.argv[1]) if len(sys.argv) > 1 else Path('scripts/test_dataset.jsonl')
if not p.exists():
    print('Dataset file not found:', p)
    raise SystemExit(2)

missing_meta_count = {k: 0 for k in REQUIRED_META}
missing_messages = 0
total = 0
bad_json = 0
sample_missing = []

with p.open('r', encoding='utf-8', errors='replace') as f:
    for i, line in enumerate(f, start=1):
        line = line.strip()
        if not line:
            continue
        total += 1
        try:
            obj = json.loads(line)
        except Exception as e:
            bad_json += 1
            if len(sample_missing) < 5:
                sample_missing.append(('json_error', i, str(e), line[:200]))
            continue
        meta = obj.get('meta')
        if not isinstance(meta, dict):
            missing_messages += 1
            if len(sample_missing) < 5:
                sample_missing.append(('no_meta', i, obj))
            continue
        for k in REQUIRED_META:
            if k not in meta:
                missing_meta_count[k] += 1
                if len(sample_missing) < 5:
                    sample_missing.append(('missing_field', i, k, meta.get('path_relative')))
        # messages structure: expect list with at least 2 entries and at least one user->assistant pair
        messages = obj.get('messages')
        ok_msgs = True
        if not isinstance(messages, list) or len(messages) < 2:
            ok_msgs = False
        else:
            # find a user then assistant following
            found_pair = False
            for idx in range(len(messages) - 1):
                if messages[idx].get('role') == 'user' and messages[idx + 1].get('role') == 'assistant':
                    found_pair = True
                    break
            if not found_pair:
                ok_msgs = False
        if not ok_msgs:
            missing_messages += 1
            if len(sample_missing) < 5:
                sample_missing.append(('bad_messages', i, meta.get('path_relative')))

print('Dataset file:', p)
print('Total records:', total)
print('Bad JSON lines:', bad_json)
print('Entries with missing/invalid messages:', missing_messages)
print('Missing meta field counts:')
for k, v in missing_meta_count.items():
    print(' ', k, ':', v)

if sample_missing:
    print('\nSamples of problems (up to 5):')
    for s in sample_missing:
        print(' ', s)

# exit code 0 if no issues
issues = bad_json + missing_messages + sum(missing_meta_count.values())
if issues == 0:
    print('\nValidation PASSED: all required fields present and messages look OK')
    sys.exit(0)
else:
    print(f'\nValidation FAILED: total issues approx {issues}')
    sys.exit(1)
