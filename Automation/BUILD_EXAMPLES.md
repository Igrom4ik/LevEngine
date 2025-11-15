# HuyEngine - Примеры использования системы сборки

## 📚 Содержание

1. [Базовые примеры](#базовые-примеры)
2. [Для Visual Studio](#для-visual-studio)
3. [Для CLion](#для-clion)
4. [Для командной строки](#для-командной-строки)
5. [Продвинутые сценарии](#продвинутые-сценарии)
6. [CI/CD интеграция](#cicd-интеграция)

## Базовые примеры

### Первая сборка проекта

```batch
# Запустить интерактивное меню
build.bat

# В меню:
# 1. Выбрать "Сборка"
# 2. Выбрать "Rebuild"
```

Или одной командой:

```batch
build.bat rebuild
```

### Быстрая сборка Debug версии

```batch
build.bat -c debug rebuild
```

### Быстрая сборка Release версии

```batch
build.bat -c release rebuild
```

### Запуск приложения

```batch
build.bat run
```

### Форматирование кода

```batch
build.bat format
```

## Для Visual Studio

### Генерация проекта Visual Studio

```batch
# Генерация для Visual Studio 2022
build.bat generate --ide vs --build-system msbuild

# После этого откройте файл .sln в папке vs-build/
```

### Сборка через MSBuild

```batch
# Debug сборка
build.bat build --ide vs --build-system msbuild --config debug

# Release сборка
build.bat build --ide vs --build-system msbuild --config release
```

### Полный цикл для Visual Studio

```batch
# 1. Генерация
build.bat generate --ide vs --build-system msbuild

# 2. Открыть в Visual Studio
start vs-build\HuyEngine.sln

# 3. Или собрать из командной строки
build.bat build --ide vs --build-system msbuild
```

### Быстрые команды для VS

```batch
# Генерация + сборка Debug
build.bat -i vs -b msbuild -c debug rebuild

# Только сборка Release (если уже сгенерировано)
build.bat -i vs -b msbuild -c release build
```

## Для CLion

### Генерация проекта CLion

```batch
# Debug версия
build.bat generate --ide clion --config debug

# Release версия
build.bat generate --ide clion --config release
```

CLion автоматически использует папки:
- `cmake-build-debug` для Debug
- `cmake-build-release` для Release

### Сборка для CLion

```batch
# Debug
build.bat -i clion -c debug build

# Release
build.bat -i clion -c release build
```

### Использование с CLion IDE

1. Откройте проект в CLion
2. CLion автоматически выполнит CMake
3. Или используйте командную строку:

```batch
# Генерация для CLion
build.bat -i clion -c debug generate

# Открыть проект (CLion должен быть в PATH)
clion64.exe .
```

## Для командной строки

### С Ninja (рекомендуется для скорости)

```batch
# Генерация
build.bat generate --build-system ninja --config release

# Сборка
build.bat build --build-system ninja --config release

# Или все сразу
build.bat rebuild --build-system ninja --config release
```

### Параллельная сборка

По умолчанию используется 8 параллельных задач. Сборка автоматически оптимизирована.

### Быстрые команды для Ninja

```batch
# Debug сборка
build.bat -b ninja -c debug rebuild

# Release сборка
build.bat -b ninja -c release rebuild

# С подробным выводом
build.bat -b ninja -c debug --verbose build
```

## Продвинутые сценарии

### Сборка нескольких конфигураций

```batch
# Debug
build.bat -c debug generate
build.bat -c debug build

# Release  
build.bat -c release generate
build.bat -c release build

# RelWithDebInfo (Release с отладочной информацией)
build.bat -c relwithdebinfo generate
build.bat -c relwithdebinfo build
```

### Очистка и пересборка

```batch
# Очистить только текущую конфигурацию
build.bat clean

# Очистить ВСЕ папки сборки
build.bat clean-all

# Пересборка с нуля
build.bat clean-all
build.bat rebuild
```

### Сборка без очистки

```batch
# Быстрая инкрементальная сборка
build.bat build --no-clean
```

### Сборка для разных платформ

```batch
# 64-bit (по умолчанию)
build.bat -p x64 rebuild

# 32-bit
build.bat -p win32 rebuild

# ARM64
build.bat -p arm64 rebuild
```

### Использование пользовательской конфигурации

1. Скопируйте шаблон:
```batch
copy Automation\user_config.template.py Automation\user_config.py
```

2. Отредактируйте `Automation\user_config.py`:
```python
# Мои настройки по умолчанию
DEFAULT_IDE = "vs"
DEFAULT_BUILD_SYSTEM = "msbuild"
DEFAULT_CONFIGURATION = "release"
DEFAULT_PARALLEL_JOBS = 16

# Мои пути (если нужно)
USER_CMAKE_PATH = r"D:\Tools\CMake\bin\cmake.exe"
```

3. Используйте как обычно:
```batch
build.bat rebuild  # Будет использовать ваши настройки
```

## CI/CD интеграция

### GitHub Actions

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.x'
      
      - name: Install dependencies
        run: |
          choco install cmake ninja
      
      - name: Generate project
        run: python Automation/automation_new.py generate --build-system ninja --config release
      
      - name: Build project
        run: python Automation/automation_new.py build --build-system ninja --config release
      
      - name: Upload artifacts
        uses: actions/upload-artifact@v3
        with:
          name: HuyEngine-Release
          path: build/bin/HuyEngine.exe
```

### GitLab CI

```yaml
build:
  stage: build
  script:
    - python Automation/automation_new.py generate --build-system ninja --config release
    - python Automation/automation_new.py build --build-system ninja --config release
  artifacts:
    paths:
      - build/bin/
```

### Jenkins

```groovy
pipeline {
    agent any
    
    stages {
        stage('Generate') {
            steps {
                bat 'python Automation/automation_new.py generate --build-system ninja --config release'
            }
        }
        
        stage('Build') {
            steps {
                bat 'python Automation/automation_new.py build --build-system ninja --config release'
            }
        }
        
        stage('Test') {
            steps {
                bat 'python Automation/automation_new.py run'
            }
        }
    }
}
```

### Локальный скрипт автоматизации

```batch
@echo off
REM build_all_configs.bat - Собрать все конфигурации

echo Building all configurations...

REM Debug
call build.bat -c debug rebuild
if %errorlevel% neq 0 exit /b 1

REM Release
call build.bat -c release rebuild
if %errorlevel% neq 0 exit /b 1

REM RelWithDebInfo
call build.bat -c relwithdebinfo rebuild
if %errorlevel% neq 0 exit /b 1

echo All configurations built successfully!
```

## Сценарии разработки

### Ежедневная разработка

```batch
# Утро - начало работы
build.bat -c debug build

# Тестирование изменений
build.bat run

# Форматирование перед коммитом
build.bat format
git add .
git commit -m "..."
```

### Перед релизом

```batch
# Полная очистка
build.bat clean-all

# Форматирование кода
build.bat format

# Сборка Release
build.bat -c release rebuild

# Тестирование Release версии
build.bat -c release run

# Создание архива
tar -czf HuyEngine-Release.tar.gz build/bin/
```

### Отладка проблем сборки

```batch
# Подробный вывод
build.bat --verbose build

# Пересоздание с нуля
build.bat clean
build.bat generate
build.bat --verbose build

# Проверка переменных окружения
build.bat generate --verbose > build_log.txt 2>&1
```

## Советы и трюки

### Ускорение сборки

1. **Используйте Ninja** (быстрее MSBuild в 2-3 раза)
```batch
build.bat -b ninja rebuild
```

2. **Инкрементальная сборка**
```batch
build.bat build --no-clean
```

3. **Параллельная сборка** (автоматически включена)

### Работа с несколькими конфигурациями

```batch
# Держите разные конфигурации одновременно
build.bat -i clion -c debug generate   # cmake-build-debug/
build.bat -i clion -c release generate # cmake-build-release/
build.bat -i vs -b msbuild generate    # vs-build/

# Быстрое переключение
build.bat -i clion -c debug run
build.bat -i clion -c release run
```

### Интеграция с редактором

**Visual Studio Code:**
```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Debug",
            "type": "shell",
            "command": "build.bat -c debug build",
            "group": "build"
        },
        {
            "label": "Build Release",
            "type": "shell",
            "command": "build.bat -c release build",
            "group": "build"
        }
    ]
}
```

---

**Последнее обновление**: 2025-01-13  
**Версия документа**: 1.0

