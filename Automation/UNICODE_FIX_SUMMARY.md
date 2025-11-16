# Исправление проблемы с Unicode в системе автоматизации сборки

## Проблема

Настройка `stdout` и `stderr` для поддержки UTF-8 выполнялась в каждом модуле системы автоматизации:

- `automation_new.py`
- `build_config.py`
- `build_actions.py`
- `build_menu.py`

Это приводило к множественным попыткам переконфигурирования потоков вывода, что могло вызывать конфликты.

## Решение

Настройка UTF-8 кодировки для `stdout` и `stderr` должна выполняться только **один раз** в главном модуле (
`automation_new.py`), который является точкой входа в систему.

## Внесённые изменения

### 1. `automation_new.py` (главный модуль)

**Оставлено без изменений** - содержит настройку Unicode:

```python
# Fix Unicode output for Windows console
if sys.platform == "win32":
    try:
        sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
        sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')
    except Exception:
        pass
```

### 2. `build_config.py`

**Удалено:**

- Импорт `io`
- Блок настройки Unicode (строки 12-20)

**Осталось:**

```python
import os
import sys
from enum import Enum
from pathlib import Path
```

### 3. `build_actions.py`

**Удалено:**

- Импорт `io`
- Блок настройки Unicode (строки 12-20)

**Осталось:**

```python
import os
import shutil
import subprocess
import sys
```

### 4. `build_menu.py`

**Удалено:**

- Импорт `io`
- Блок настройки Unicode (строки 10-18)

**Осталось:**

```python
import os
import sys
```

## Результат

✓ Настройка UTF-8 выполняется только один раз при запуске главного модуля
✓ Все модули корректно компилируются без ошибок
✓ Русские символы и специальные символы Unicode отображаются корректно
✓ Устранены потенциальные конфликты при множественной настройке потоков

## Тестирование

Проверено:

```bash
python automation_new.py --help
✓ Выводит справку с русскими символами корректно

python automation_new.py
✓ Запускает интерактивное меню с корректным отображением Unicode
```

## Рекомендации

- При создании новых модулов в системе автоматизации **не добавляйте** настройку Unicode
- Настройка потоков должна быть только в главной точке входа (`automation_new.py`)
- Для тестов или отдельных скриптов, запускаемых независимо, можно добавить локальную настройку Unicode

## Дата исправления

2025-11-13

