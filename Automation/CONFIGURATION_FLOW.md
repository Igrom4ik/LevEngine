# Процесс выбора и применения конфигурации сборки

## 📋 Обзор

Когда вы выбираете ту или иную конфигурацию в системе автоматизации сборки HuyEngine, происходит следующий процесс:

---

## 🎯 Шаг 1: Выбор параметров конфигурации

### Параметры, которые вы настраиваете:

#### 1. **IDE** (Интегрированная среда разработки)

```python
class IDE(Enum):
    VISUAL_STUDIO = "Visual Studio"
    CLION = "CLion"
    VSCODE = "VSCode"
    MANUAL = "Manual (Command Line)"
```

**Влияние:**

- Определяет **имя папки сборки**
- Определяет **стиль генерации проекта**

**Примеры папок:**

- `CLion` + `Debug` → `cmake-build-debug`
- `CLion` + `Release` → `cmake-build-release`
- `Visual Studio` → `vs-build`
- `Manual` + `MSVC` → `build-msvc`
- `Manual` + `Clang-CL` → `build-clang-cl`
- `Manual` + `MinGW` → `build-mingw`

---

#### 2. **Build System** (Система сборки)

```python
class BuildSystem(Enum):
    NINJA = "Ninja"
    MSBUILD = "MSBuild (Visual Studio)"
    MAKE = "Unix Makefiles"
```

**Влияние:**

- Определяет **CMake генератор** (`-G` параметр)
- Определяет **формат файлов проекта**

**Что создается:**

- `Ninja` → `build.ninja` (быстрая параллельная сборка)
- `MSBuild` → `.sln` + `.vcxproj` (Visual Studio решения)
- `Make` → `Makefile` (Unix-стиль)

---

#### 3. **Configuration** (Конфигурация сборки)

```python
class Configuration(Enum):
    DEBUG = "Debug"
    RELEASE = "Release"
    RELWITHDEBINFO = "RelWithDebInfo"
    MINSIZEREL = "MinSizeRel"
```

**Влияние:**

- Определяет **флаги оптимизации компилятора**
- Определяет **наличие отладочной информации**

**Типичные флаги:**

- `Debug`: `/Od /Zi /RTC1` (без оптимизации, полная отладка)
- `Release`: `/O2 /DNDEBUG` (максимальная оптимизация)
- `RelWithDebInfo`: `/O2 /Zi` (оптимизация + отладочная информация)
- `MinSizeRel`: `/Os /DNDEBUG` (оптимизация размера)

---

#### 4. **Platform** (Целевая платформа)

```python
class Platform(Enum):
    X64 = "x64"
    WIN32 = "Win32"
    ARM64 = "ARM64"
```

**Влияние:**

- Определяет **целевую архитектуру** исполняемого файла
- Настраивает **компилятор и линкер** для нужной платформы

---

#### 5. **Toolchain** (Набор инструментов компиляции)

```python
class Toolchain(Enum):
    MSVC = "MSVC (cl)"
    CLANG_CL = "Clang-CL (LLVM with MSVC)"
    MINGW_GCC = "MinGW (GCC)"
```

**Влияние:**

- Определяет **компилятор** C/C++
- Определяет **toolchain file** для CMake
- Влияет на **совместимость библиотек**

**Пути к toolchain файлам:**

- `MSVC` → `Automation/CMAKE/Toolchains/toolchain-msvc.cmake`
- `Clang-CL` → `Automation/CMAKE/Toolchains/toolchain-clang-cl.cmake`
- `MinGW` → `Automation/CMAKE/Toolchains/toolchain-mingw.cmake`

---

## 🔄 Шаг 2: Сохранение конфигурации

Когда вы выбираете параметр в меню, происходит обновление глобального состояния:

```python
def menu_configure(self):
    # Выбор IDE
    ide = self.select_from_enum(IDE, "Select IDE:")
    if ide:
        BuildConfig.current_ide = ide  # ← Сохранение в BuildConfig

    # Выбор build system
    build_sys = self.select_from_enum(BuildSystem, "Select build system:")
    if build_sys:
        BuildConfig.current_build_system = build_sys

    # ... и так далее для остальных параметров
```

**Где хранится:**

```python
class BuildConfig:
    # Текущие настройки (глобальное состояние)
    current_ide = IDE.MANUAL
    current_build_system = BuildSystem.NINJA
    current_configuration = Configuration.DEBUG
    current_platform = Platform.X64
    current_toolchain = Toolchain.MSVC
```

---

## 🏗️ Шаг 3: Генерация проекта

Когда вы выбираете **Generate**, система:

### 3.1. Определяет папку сборки

```python
def get_build_folder():
    if BuildConfig.current_ide == IDE.CLION:
        if BuildConfig.current_configuration == Configuration.DEBUG:
            return "cmake-build-debug"
        # ...
    elif BuildConfig.current_ide == IDE.VISUAL_STUDIO:
        return "vs-build"
    else:
        # По toolchain
        if tc == Toolchain.MSVC:
            return "build-msvc"
        # ...
```

### 3.2. Создает CMake команду

```python
def get_cmake_generate_command():
    args = [
        '"cmake"',
        '-S "C:/DEVOPS/HuyEngine"',  # Исходники
        '-B "build-msvc"',  # Папка сборки
        '-G "Ninja"',  # Генератор
        '-DCMAKE_BUILD_TYPE=Debug',  # Конфигурация
        '-DCMAKE_TOOLCHAIN_FILE="toolchain-msvc.cmake"',  # Toolchain
        '-DCMAKE_MAKE_PROGRAM="ninja.exe"',
    ]
    return ' '.join(args)
```

### 3.3. Настраивает окружение (при необходимости)

Для MSVC и Clang-CL:

```python
def setup_msvc_environment():
    # Запускает vcvarsall.bat для настройки PATH, INCLUDE, LIB
    command = f'"{vcvarsall.bat}" x64 && set'
    # Получает переменные окружения
    # Возвращает настроенный env
```

### 3.4. Выполняет команду CMake

```bash
cmake -S "C:/DEVOPS/HuyEngine" -B "build-msvc" -G "Ninja" ...
```

**Результат:**

- ✅ Создана папка сборки (например, `build-msvc/`)
- ✅ Сгенерированы файлы проекта (`build.ninja`, `CMakeCache.txt`)
- ✅ CMake настроил компилятор и зависимости

---

## 🔨 Шаг 4: Сборка проекта

Когда вы выбираете **Build**, система:

### 4.1. Создает команду сборки

```python
def get_cmake_build_command():
    args = [
        '"cmake"',
        '--build "build-msvc"',  # Папка с проектом
        '--config Debug',  # Конфигурация
        '--clean-first',  # Очистка перед сборкой
        '-j 8',  # 8 параллельных задач
    ]
    return ' '.join(args)
```

### 4.2. Выполняет сборку

```bash
cmake --build "build-msvc" --config Debug --clean-first -j 8
```

**Что происходит внутри:**

- CMake вызывает выбранную систему сборки (Ninja/MSBuild)
- Компилируются все `.cpp` файлы в `.obj`
- Линкуются объектные файлы в исполняемый файл
- Создается `HuyEngine.exe` в папке `build-msvc/bin/`

---

## 📊 Диаграмма потока данных

```
┌─────────────────────────────────────────────────────────────┐
│                    ВЫБОР КОНФИГУРАЦИИ                       │
│  [IDE] [BuildSystem] [Configuration] [Platform] [Toolchain] │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
              ┌──────────────────────┐
              │   BuildConfig        │
              │  (Глобальное         │
              │   состояние)         │
              └──────────┬───────────┘
                         │
         ┌───────────────┴───────────────┐
         │                               │
         ▼                               ▼
┌────────────────┐            ┌──────────────────┐
│   GENERATE     │            │      BUILD       │
└────────┬───────┘            └────────┬─────────┘
         │                              │
         ▼                              ▼
┌─────────────────────┐      ┌──────────────────────┐
│ get_build_folder()  │      │ get_cmake_build_cmd()│
│ get_cmake_gen_cmd() │      │ setup_environment()  │
│ setup_environment() │      │ run_command()        │
│ run_command()       │      └──────────┬───────────┘
└─────────┬───────────┘                 │
          │                             │
          ▼                             ▼
   ┌─────────────────┐          ┌──────────────┐
   │  CMake генерация│          │  CMake сборка│
   │  ninja files    │          │  компиляция  │
   └─────────┬───────┘          └──────┬───────┘
             │                         │
             ▼                         ▼
      build-msvc/              build-msvc/bin/
      build.ninja              HuyEngine.exe
```

---

## 🎯 Примеры конфигураций

### Пример 1: CLion Debug

```
IDE: CLion
BuildSystem: Ninja
Configuration: Debug
Platform: x64
Toolchain: MSVC

Результат:
- Папка: cmake-build-debug/
- Генератор: Ninja
- Флаги: /Od /Zi /RTC1
- Компилятор: cl.exe (MSVC)
- Выход: cmake-build-debug/bin/HuyEngine.exe
```

### Пример 2: Visual Studio Release

```
IDE: Visual Studio
BuildSystem: MSBuild
Configuration: Release
Platform: x64
Toolchain: MSVC

Результат:
- Папка: vs-build/
- Генератор: Visual Studio 17 2022
- Флаги: /O2 /DNDEBUG
- Компилятор: cl.exe (MSVC)
- Файлы: HuyEngine.sln, HuyEngine.vcxproj
- Выход: vs-build/bin/Release/HuyEngine.exe
```

### Пример 3: Manual Clang-CL

```
IDE: Manual
BuildSystem: Ninja
Configuration: Release
Platform: x64
Toolchain: Clang-CL

Результат:
- Папка: build-clang-cl/
- Генератор: Ninja
- Флаги: -O2 -DNDEBUG
- Компилятор: clang-cl.exe (LLVM)
- Toolchain: toolchain-clang-cl.cmake
- Выход: build-clang-cl/bin/HuyEngine.exe
```

### Пример 4: MinGW GCC

```
IDE: Manual
BuildSystem: Ninja
Configuration: Debug
Platform: x64
Toolchain: MinGW GCC

Результат:
- Папка: build-mingw/
- Генератор: Ninja
- Флаги: -g -O0
- Компилятор: g++.exe (MinGW)
- Toolchain: toolchain-mingw.cmake
- Выход: build-mingw/bin/HuyEngine.exe
```

---

## 🔧 Технические детали

### Toolchain файлы

Каждый toolchain файл настраивает CMake для конкретного компилятора:

**toolchain-msvc.cmake:**

```cmake
set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)
set(CMAKE_CXX_FLAGS "/EHsc /W4")
```

**toolchain-clang-cl.cmake:**

```cmake
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_CXX_FLAGS "-Wextra -Wall")
```

**toolchain-mingw.cmake:**

```cmake
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
```

---

## 📝 Резюме

### Что происходит при выборе конфигурации:

1. ✅ **Обновляется глобальное состояние** `BuildConfig`
2. ✅ **Определяется папка сборки** на основе IDE/Toolchain
3. ✅ **Создается команда CMake** с нужными параметрами
4. ✅ **Настраивается окружение** (vcvarsall для MSVC)
5. ✅ **Генерируются файлы проекта** (Ninja/MSBuild/Make)
6. ✅ **При сборке компилируется проект** с выбранными флагами
7. ✅ **Создается исполняемый файл** в папке bin/

### Ключевые преимущества:

- 🎯 **Изоляция сборок**: разные toolchain'ы в разных папках
- 🚀 **Гибкость**: можно быстро переключаться между конфигурациями
- 🔧 **Автоматизация**: все настройки применяются автоматически
- 📦 **Воспроизводимость**: одинаковая конфигурация → одинаковый результат

---

## 📚 Дополнительные материалы

- `BUILD_SYSTEM_README.md` - Подробное описание системы сборки
- `BUILD_EXAMPLES.md` - Примеры использования
- `BUILD_FAQ.md` - Часто задаваемые вопросы
- `../docs/UNICODE_FIX_README.md` - Исправления Unicode

**Дата создания:** 2025-11-13
