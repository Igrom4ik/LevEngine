# Визуальная схема выбора конфигурации

## 🎨 Полная схема процесса сборки

```
╔═══════════════════════════════════════════════════════════════════════════╗
║                     ИНТЕРАКТИВНОЕ МЕНЮ / CLI                              ║
╚═══════════════════════════════════════════════════════════════════════════╝
                                    │
                    ┌───────────────┼───────────────┐
                    │               │               │
                    ▼               ▼               ▼
            ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
            │     IDE      │ │ Build System │ │Configuration │
            ├──────────────┤ ├──────────────┤ ├──────────────┤
            │• Visual Studio│ │• Ninja       │ │• Debug       │
            │• CLion       │ │• MSBuild     │ │• Release     │
            │• VSCode      │ │• Make        │ │• RelWithDeb  │
            │• Manual      │ └──────────────┘ │• MinSizeRel  │
            └──────────────┘                  └──────────────┘
                    │               │               │
                    │               │               │
            ┌──────────────┐ ┌──────────────┐      │
            │   Platform   │ │  Toolchain   │      │
            ├──────────────┤ ├──────────────┤      │
            │• x64         │ │• MSVC        │      │
            │• Win32       │ │• Clang-CL    │      │
            │• ARM64       │ │• MinGW GCC   │      │
            └──────────────┘ └──────────────┘      │
                    │               │               │
                    └───────────────┴───────────────┘
                                    │
                                    ▼
        ╔═══════════════════════════════════════════════════════╗
        ║            BuildConfig (Глобальное состояние)          ║
        ╠═══════════════════════════════════════════════════════╣
        ║  current_ide: IDE.MANUAL                              ║
        ║  current_build_system: BuildSystem.NINJA              ║
        ║  current_configuration: Configuration.DEBUG           ║
        ║  current_platform: Platform.X64                       ║
        ║  current_toolchain: Toolchain.MSVC                    ║
        ╚═══════════════════════════════════════════════════════╝
                                    │
                    ┌───────────────┴───────────────┐
                    │                               │
                    ▼                               ▼
        ┌───────────────────────┐      ┌───────────────────────┐
        │   GENERATE PROJECT    │      │    BUILD PROJECT      │
        └───────────┬───────────┘      └───────────┬───────────┘
                    │                               │
                    ▼                               ▼
        ╔═══════════════════════╗      ╔═══════════════════════╗
        ║ get_build_folder()    ║      ║ get_cmake_build_cmd() ║
        ╠═══════════════════════╣      ╠═══════════════════════╣
        ║ Определяет папку:     ║      ║ Создает команду:      ║
        ║                       ║      ║                       ║
        ║ CLion+Debug           ║      ║ cmake --build         ║
        ║   → cmake-build-debug ║      ║ "build-msvc"          ║
        ║                       ║      ║ --config Debug        ║
        ║ Manual+MSVC           ║      ║ --clean-first         ║
        ║   → build-msvc        ║      ║ -j 8                  ║
        ║                       ║      ║                       ║
        ║ Manual+Clang          ║      ╚═══════════════════════╝
        ║   → build-clang-cl    ║                  │
        ║                       ║                  │
        ║ Manual+MinGW          ║                  ▼
        ║   → build-mingw       ║      ╔═══════════════════════╗
        ╚═══════════════════════╝      ║ setup_environment()   ║
                    │                  ╠═══════════════════════╣
                    ▼                  ║ Для MSVC/Clang-CL:    ║
        ╔═══════════════════════╗      ║ Запуск vcvarsall.bat  ║
        ║get_cmake_generate_cmd()║      ║                       ║
        ╠═══════════════════════╣      ║ Настройка:            ║
        ║ Создает команду:      ║      ║ • PATH                ║
        ║                       ║      ║ • INCLUDE             ║
        ║ cmake -S <source>     ║      ║ • LIB                 ║
        ║ -B <build_folder>     ║      ║ • LIBPATH             ║
        ║ -G "Ninja"            ║      ╚═══════════════════════╝
        ║ -DCMAKE_BUILD_TYPE=   ║                  │
        ║    Debug              ║                  │
        ║ -DCMAKE_TOOLCHAIN_    ║                  ▼
        ║    FILE=toolchain-    ║      ╔═══════════════════════╗
        ║    msvc.cmake         ║      ║   run_command()       ║
        ║ -DCMAKE_MAKE_PROGRAM= ║      ╠═══════════════════════╣
        ║    ninja.exe          ║      ║ Выполняет сборку      ║
        ╚═══════════════════════╝      ╚═══════════════════════╝
                    │                              │
                    ▼                              ▼
        ╔═══════════════════════╗      ╔═══════════════════════╗
        ║ setup_environment()   ║      ║   КОМПИЛЯЦИЯ          ║
        ╠═══════════════════════╣      ╠═══════════════════════╣
        ║ Для MSVC/Clang-CL:    ║      ║ Ninja/MSBuild         ║
        ║ Запуск vcvarsall.bat  ║      ║ вызывает компилятор   ║
        ║                       ║      ║                       ║
        ║ Для MinGW:            ║      ║ ┌─────────────────┐   ║
        ║ Добавляет MinGW       ║      ║ │ main.cpp        │   ║
        ║ в PATH                ║      ║ │ module1.cpp     │   ║
        ╚═══════════════════════╝      ║ │ module2.cpp     │   ║
                    │                  ║ └─────────────────┘   ║
                    ▼                  ║         │             ║
        ╔═══════════════════════╗      ║         ▼             ║
        ║   run_command()       ║      ║ ┌─────────────────┐   ║
        ╠═══════════════════════╣      ║ │ main.obj        │   ║
        ║ Выполняет генерацию   ║      ║ │ module1.obj     │   ║
        ╚═══════════════════════╝      ║ │ module2.obj     │   ║
                    │                  ║ └─────────────────┘   ║
                    ▼                  ║         │             ║
        ╔═══════════════════════╗      ║         ▼             ║
        ║  CMAKE ГЕНЕРАЦИЯ      ║      ║ ┌─────────────────┐   ║
        ╠═══════════════════════╣      ║ │ ЛИНКОВКА        │   ║
        ║ Создает:              ║      ║ └─────────────────┘   ║
        ║ • CMakeCache.txt      ║      ║         │             ║
        ║ • build.ninja         ║      ║         ▼             ║
        ║   (или .sln/.vcxproj) ║      ║ ┌─────────────────┐   ║
        ║ • cmake_install.cmake ║      ║ │ HuyEngine.exe   │   ║
        ║ • CMakeFiles/         ║      ║ └─────────────────┘   ║
        ╚═══════════════════════╝      ╚═══════════════════════╝
                    │                              │
                    └──────────────┬───────────────┘
                                   ▼
                    ╔═══════════════════════════════╗
                    ║       РЕЗУЛЬТАТ СБОРКИ        ║
                    ╠═══════════════════════════════╣
                    ║ build-msvc/                   ║
                    ║ ├── bin/                      ║
                    ║ │   └── HuyEngine.exe         ║
                    ║ ├── CMakeCache.txt            ║
                    ║ ├── build.ninja               ║
                    ║ └── CMakeFiles/               ║
                    ╚═══════════════════════════════╝
```

---

## 🔄 Схема переключения между конфигурациями

```
┌─────────────────────────────────────────────────────────────────┐
│                     ТЕКУЩАЯ КОНФИГУРАЦИЯ                        │
│  IDE: Manual  │ Build: Ninja │ Config: Debug │ Toolchain: MSVC │
└─────────────────────────────────────────────────────────────────┘
                                │
                    ┌───────────┴───────────┐
                    │   Изменить на:        │
                    │   Toolchain: Clang-CL │
                    └───────────┬───────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│ BuildConfig.current_toolchain = Toolchain.CLANG_CL             │
└─────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│ get_build_folder() → "build-clang-cl"  (ИЗМЕНИЛОСЬ!)           │
│ get_toolchain_file() → "toolchain-clang-cl.cmake"              │
└─────────────────────────────────────────────────────────────────┘
                                │
                                ▼
                    ┌───────────────────────┐
                    │   Запуск GENERATE     │
                    └───────────┬───────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│ НОВАЯ ПАПКА СБОРКИ: build-clang-cl/                            │
│ • Полностью изолирована от build-msvc/                          │
│ • Свой CMakeCache.txt                                           │
│ • Свои скомпилированные объектные файлы                         │
└─────────────────────────────────────────────────────────────────┘
```

---

## 📊 Матрица влияния параметров

```
┌──────────────┬────────────────┬──────────────────┬─────────────────┐
│  ПАРАМЕТР    │  ВЛИЯЕТ НА     │   ПРИМЕР         │  КАК ВЛИЯЕТ     │
├──────────────┼────────────────┼──────────────────┼─────────────────┤
│              │                │                  │                 │
│  IDE         │ Имя папки      │ CLion → cmake-   │ Определяет      │
│              │ сборки         │ build-debug      │ структуру папок │
│              │                │                  │                 │
├──────────────┼────────────────┼──────────────────┼─────────────────┤
│              │                │                  │                 │
│ Build System │ CMake          │ Ninja →          │ Какие файлы     │
│              │ генератор      │ build.ninja      │ создаются       │
│              │                │                  │                 │
├──────────────┼────────────────┼──────────────────┼─────────────────┤
│              │                │                  │                 │
│Configuration │ Флаги          │ Debug →          │ Оптимизация и   │
│              │ компилятора    │ /Od /Zi          │ отладка         │
│              │                │                  │                 │
├──────────────┼────────────────┼──────────────────┼─────────────────┤
│              │                │                  │                 │
│  Platform    │ Целевая        │ x64 →            │ Архитектура     │
│              │ архитектура    │ 64-битный exe    │ исполняемого    │
│              │                │                  │ файла           │
│              │                │                  │                 │
├──────────────┼────────────────┼──────────────────┼─────────────────┤
│              │                │                  │                 │
│  Toolchain   │ Компилятор     │ MSVC → cl.exe    │ Какой компилятор│
│              │ Toolchain file │ Clang → clang-cl │ используется    │
│              │ Имя папки      │ MinGW → g++      │                 │
│              │                │                  │                 │
└──────────────┴────────────────┴──────────────────┴─────────────────┘
```

---

## 🎯 Примеры команд для разных конфигураций

### Конфигурация 1: MSVC Debug

```bash
cmake -S "C:/DEVOPS/HuyEngine" \
      -B "build-msvc" \
      -G "Ninja" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_TOOLCHAIN_FILE="toolchain-msvc.cmake" \
      -DCMAKE_MAKE_PROGRAM="ninja.exe"

# Результат:
# build-msvc/
#   ├── build.ninja
#   └── CMakeCache.txt (компилятор: cl.exe)
```

### Конфигурация 2: Clang-CL Release

```bash
cmake -S "C:/DEVOPS/HuyEngine" \
      -B "build-clang-cl" \
      -G "Ninja" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="toolchain-clang-cl.cmake" \
      -DCMAKE_MAKE_PROGRAM="ninja.exe"

# Результат:
# build-clang-cl/
#   ├── build.ninja
#   └── CMakeCache.txt (компилятор: clang-cl.exe)
```

### Конфигурация 3: MinGW Debug

```bash
cmake -S "C:/DEVOPS/HuyEngine" \
      -B "build-mingw" \
      -G "Ninja" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_TOOLCHAIN_FILE="toolchain-mingw.cmake" \
      -DCMAKE_MAKE_PROGRAM="ninja.exe" \
      -DCMAKE_SYSTEM_NAME=Windows

# Результат:
# build-mingw/
#   ├── build.ninja
#   └── CMakeCache.txt (компилятор: g++.exe)
```

---

## 🔍 Детализация: Что делает каждая функция

```
BuildMenu.menu_configure()
  │
  ├─► select_from_enum(IDE, ...)
  │   └─► Показывает список: Visual Studio, CLion, VSCode, Manual
  │       Пользователь выбирает → сохраняется в BuildConfig.current_ide
  │
  ├─► select_from_enum(BuildSystem, ...)
  │   └─► Показывает список: Ninja, MSBuild, Make
  │       Пользователь выбирает → сохраняется в BuildConfig.current_build_system
  │
  ├─► select_from_enum(Configuration, ...)
  │   └─► Показывает список: Debug, Release, RelWithDebInfo, MinSizeRel
  │       Пользователь выбирает → сохраняется в BuildConfig.current_configuration
  │
  ├─► select_from_enum(Platform, ...)
  │   └─► Показывает список: x64, Win32, ARM64
  │       Пользователь выбирает → сохраняется в BuildConfig.current_platform
  │
  └─► select_from_enum(Toolchain, ...)
      └─► Показывает список: MSVC, Clang-CL, MinGW GCC
          Пользователь выбирает → сохраняется в BuildConfig.current_toolchain

После выбора всех параметров:
BuildConfig содержит полную конфигурацию сборки!
```

---

## 💡 Ключевые моменты

### 1. Изоляция сборок

```
build-msvc/          ← MSVC компилятор
build-clang-cl/      ← Clang-CL компилятор
build-mingw/         ← MinGW компилятор
cmake-build-debug/   ← CLion Debug
cmake-build-release/ ← CLion Release

Каждая папка полностью независима!
Можно переключаться без конфликтов.
```

### 2. Автоматическая настройка окружения

```
MSVC/Clang-CL:
  vcvarsall.bat x64  ← Настраивает PATH, INCLUDE, LIB
  
MinGW:
  Добавляет C:\msys64\mingw64\bin в PATH
```

### 3. Гибкость команд

```
CLI:
  python automation_new.py -t msvc -c debug generate
  
Меню:
  1. Configuration → Select Toolchain → MSVC
  2. Build → Generate
```

---

**Дата создания:** 2025-11-13

