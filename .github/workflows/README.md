# GitHub Actions Workflows

Этот каталог содержит конфигурации GitHub Actions для автоматической проверки и сборки HuyEngine.

## 📋 Доступные Workflows

### 1. Clang-format Check (`clang-format-checker.yml`)

**Триггеры:**

- Push в ветку `master`
- Pull Request в ветку `master`

**Описание:**
Проверяет соответствие кода стандартам форматирования, определенным в `.clang-format`.

**Настройки:**

- Версия clang-format: 19
- Проверяемые директории: `Engine`

**Статус:**
[![Clang-format check](../../actions/workflows/clang-format-checker.yml/badge.svg)](../../actions/workflows/clang-format-checker.yml)

---

### 2. Windows Build (`windows-build.yml`)

**Триггеры:**

- Push в ветки `master`, `develop`
- Pull Request в ветки `master`, `develop`

**Описание:**
Сборка проекта на Windows с использованием MSVC компилятора и Ninja генератора.

**Матрица сборки:**

- Конфигурации: Debug, Release
- Компилятор: MSVC (автоматически через msvc-dev-cmd)
- Генератор: Ninja
- CMake: 3.31.6

**Пресеты:**

- Debug → `otladka` (cmake-build-debug/)
- Release → `reliz` (cmake-build-release/)

**Артефакты:**

- `HuyEngine-Windows-Debug`: .exe, .lib, .pdb файлы
- `HuyEngine-Windows-Release`: .exe, .lib файлы

**Статус:**
[![Windows Build](../../actions/workflows/windows-build.yml/badge.svg)](../../actions/workflows/windows-build.yml)

---

### 3. Linux Build (`linux-build.yml`)

**Триггеры:**

- Push в ветки `master`, `develop`
- Pull Request в ветки `master`, `develop`

**Описание:**
Сборка проекта на Linux с использованием GCC и Clang компиляторов.

**Матрица сборки:**

- Конфигурации: Debug, Release
- Компиляторы: GCC 13, Clang 18
- Генератор: Ninja

**Всего комбинаций:** 4 (2 конфигурации × 2 компилятора)

**Артефакты:**

- `HuyEngine-Linux-gcc-Debug`
- `HuyEngine-Linux-gcc-Release`
- `HuyEngine-Linux-clang-Debug`
- `HuyEngine-Linux-clang-Release`

**Статус:**
[![Linux Build](../../actions/workflows/linux-build.yml/badge.svg)](../../actions/workflows/linux-build.yml)

---

## 🔧 Использование

### Локальная проверка форматирования

Перед коммитом можно проверить форматирование локально:

```powershell
# Проверка форматирования
.\Automation\build.bat format

# Или напрямую через clang-format
clang-format -i -style=file Engine/Source/**/*.cpp Engine/Source/**/*.hpp
```

### Локальная сборка (аналог CI)

**Windows:**

```powershell
# Debug
.\Automation\build.bat -i clion -c debug rebuild

# Release
.\Automation\build.bat -i clion -c release rebuild
```

**Linux (если есть WSL):**

```bash
# Debug
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=23
cmake --build build

# Release
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=23
cmake --build build
```

---

## 📦 Артефакты

Все успешные сборки создают артефакты, которые можно скачать на странице Actions:

1. Перейдите на вкладку **Actions** в GitHub
2. Выберите нужный workflow run
3. Прокрутите вниз до секции **Artifacts**
4. Скачайте нужный артефакт

**Содержимое артефактов:**

- Исполняемые файлы (`.exe` / бинарники)
- Статические библиотеки (`.lib` / `.a`)
- Debug символы (`.pdb`) - только для Windows Debug

---

## 🚀 Добавление новых workflows

### Структура workflow файла

```yaml
name: Workflow Name

on:
  push:
    branches: [ "master", "develop" ]
  pull_request:
    branches: [ "master", "develop" ]

jobs:
  job-name:
    name: Job Display Name
    runs-on: ubuntu-latest  # или windows-latest, macos-latest

    steps:
      - uses: actions/checkout@v4

      # Ваши шаги...
```

### Рекомендации

1. **Именование**: Используйте понятные имена для workflows и jobs
2. **Триггеры**: Настройте правильные ветки для push/PR
3. **Матрицы**: Используйте matrix для тестирования нескольких конфигураций
4. **Артефакты**: Загружайте только необходимые файлы
5. **Кеширование**: Используйте `actions/cache` для ускорения сборки

---

## 🐛 Устранение проблем

### Workflow не запускается

- Проверьте синтаксис YAML (используйте VS Code с расширением YAML)
- Убедитесь, что файл находится в `.github/workflows/`
- Проверьте, что триггеры настроены правильно

### Сборка падает

1. **Проверьте логи** в GitHub Actions
2. **Воспроизведите локально** с теми же параметрами
3. **Проверьте зависимости** (CMake, компиляторы, библиотеки)

### Clang-format проверка не проходит

```powershell
# Автоматически отформатируйте код
.\Automation\build.bat format

# Или вручную
clang-format -i Engine/Source/**/*.{ cpp, hpp }
```

---

## 📚 Документация

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [CMake Actions](https://github.com/marketplace?type=actions&query=cmake)
- [Clang-format Action](https://github.com/jidicula/clang-format-action)

---

## ✅ Статус всех workflows

| Workflow           | Статус                                                                                                                                | Описание                |
|--------------------|---------------------------------------------------------------------------------------------------------------------------------------|-------------------------|
| Clang-format check | [![Clang-format check](../../actions/workflows/clang-format-checker.yml/badge.svg)](../../actions/workflows/clang-format-checker.yml) | Проверка форматирования |
| Windows Build      | [![Windows Build](../../actions/workflows/windows-build.yml/badge.svg)](../../actions/workflows/windows-build.yml)                    | Сборка на Windows       |
| Linux Build        | [![Linux Build](../../actions/workflows/linux-build.yml/badge.svg)](../../actions/workflows/linux-build.yml)                          | Сборка на Linux         |

---

<div align="center">
  <em>📝 Документация создана с помощью <strong>GitHub Copilot</strong> 🤖</em>
</div>

