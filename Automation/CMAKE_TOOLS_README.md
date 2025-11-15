# Инструменты диагностики и очистки CMake

Этот каталог содержит утилиты для диагностики и исправления проблем с конфигурацией CMake.

## 📋 Содержание

- **diagnose_build_environment.ps1** - Диагностика окружения сборки
- **clean_cmake_cache.ps1** - Очистка кэша CMake
- **CMAKE/Toolchains/toolchain-clang-cl.cmake** - Улучшенный toolchain для Clang-CL

## 🔍 diagnose_build_environment.ps1

Диагностирует окружение сборки и выявляет проблемы.

### Что проверяет:

- ✅ Наличие clang-cl в системе
- ✅ Наличие MSVC (cl.exe)
- ✅ Конфликты с MSYS2/MinGW/ucrt64
- ✅ Наличие CMake и Ninja
- ✅ Состояние кэша всех папок сборки

### Использование:

```powershell
# Запустить диагностику
.\Automation\diagnose_build_environment.ps1
```

### Пример вывода:

```
=== Диагностика окружения сборки ===

1. Проверка clang-cl:
   ✓ clang-cl найден: C:\Program Files\LLVM\bin\clang-cl.exe

2. Проверка MSVC (cl.exe):
   ✓ cl.exe найден: C:\Program Files\...\cl.exe

3. Проверка конфликтов с MSYS2/MinGW/ucrt64:
   ✓ g++ не найден (хорошо для clang-cl)

...
```

## 🧹 clean_cmake_cache.ps1

Очищает кэш CMake из папок сборки.

### Режимы работы:

#### 1. Очистка только кэша (по умолчанию)

Удаляет CMakeCache.txt и CMakeFiles/ из всех стандартных папок сборки:

```powershell
.\Automation\clean_cmake_cache.ps1
```

#### 2. Очистка конкретной папки

Удаляет кэш из указанной папки:

```powershell
# По относительному пути
.\Automation\clean_cmake_cache.ps1 -BuildDir build

# По абсолютному пути
.\Automation\clean_cmake_cache.ps1 -BuildDir "C:\DEVOPS\HuyEngine\build"
```

#### 3. Удаление всех папок сборки

Полностью удаляет все стандартные папки сборки:

```powershell
.\Automation\clean_cmake_cache.ps1 -All
```

⚠️ **Предупреждение**: Это удалит build/, build-msvc/, cmake-build-debug/, cmake-build-release/, etc.

#### 4. Режим DryRun

Показывает, что будет удалено, но не удаляет:

```powershell
.\Automation\clean_cmake_cache.ps1 -DryRun
.\Automation\clean_cmake_cache.ps1 -BuildDir build -DryRun
.\Automation\clean_cmake_cache.ps1 -All -DryRun
```

### Примеры использования:

```powershell
# Я хочу очистить только кэш, сохранив скомпилированные файлы
.\Automation\clean_cmake_cache.ps1

# Я хочу полностью переконфигурировать папку build
.\Automation\clean_cmake_cache.ps1 -BuildDir build

# Я хочу начать всё заново (удалить все папки сборки)
.\Automation\clean_cmake_cache.ps1 -All

# Я хочу посмотреть, что будет удалено
.\Automation\clean_cmake_cache.ps1 -All -DryRun
```

## 🔧 toolchain-clang-cl.cmake

Улучшенный toolchain файл для использования Clang-CL (LLVM с MSVC-совместимым интерфейсом).

### Что улучшено:

#### Автоматический поиск clang-cl

Ищет clang-cl в стандартных местах установки:

- Standalone LLVM: `C:\Program Files\LLVM\bin`
- Visual Studio 2022: `...\VC\Tools\Llvm\x64\bin`
- Visual Studio 2019: `...\VC\Tools\Llvm\x64\bin`
- Системный PATH

#### Информативные ошибки

Если clang-cl не найден, выводит понятное сообщение с инструкциями по установке.

#### Поддержка явного задания компилятора

Можно указать путь к компилятору вручную:

```powershell
cmake -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=Automation/CMAKE/Toolchains/toolchain-clang-cl.cmake `
  -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" `
  -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" `
  -B build
```

### Использование:

```powershell
# 1. Убедитесь, что clang-cl установлен
.\Automation\diagnose_build_environment.ps1

# 2. Очистите старый кэш (если есть)
.\Automation\clean_cmake_cache.ps1 -BuildDir build

# 3. Настройте окружение MSVC (опционально, но рекомендуется)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

# 4. Конфигурируйте проект
cmake -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=Automation/CMAKE/Toolchains/toolchain-clang-cl.cmake `
  -DCMAKE_BUILD_TYPE=Release `
  -B build

# 5. Соберите
cmake --build build --config Release
```

## 🚨 Типичные проблемы и решения

### Проблема: clang-cl не найден

**Симптомы**:

```
CMake Error: CMAKE_CXX_COMPILER not set, after EnableLanguage
```

или

```
clang-cl compiler not found!
```

**Решение**:

1. Установите LLVM: https://github.com/llvm/llvm-project/releases
2. ИЛИ установите Visual Studio с компонентом "C++ Clang tools for Windows"
3. Убедитесь, что clang-cl доступен:
   ```powershell
   Get-Command clang-cl
   ```

### Проблема: Конфликт с MSYS2/ucrt64

**Симптомы**:

- CMake находит g++ вместо clang-cl
- Ошибки линковки с MSVC библиотеками
- Несовместимые флаги компилятора

**Диагностика**:

```powershell
.\Automation\diagnose_build_environment.ps1
```

**Решение**:
Временно удалите MSYS2 из PATH:

```powershell
$env:PATH = ($env:PATH -split ';' | Where-Object {
    $_ -notlike "*msys*" -and $_ -notlike "*mingw*" -and $_ -notlike "*ucrt64*"
}) -join ';'
```

### Проблема: Старый кэш CMake

**Симптомы**:

- `CMAKE_CXX_COMPILER-NOTFOUND` в логах
- CMake игнорирует toolchain файл
- Изменения в CMakeLists.txt не применяются

**Решение**:

```powershell
# Очистить кэш
.\Automation\clean_cmake_cache.ps1 -BuildDir build

# Переконфигурировать
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=... -B build
```

## 📚 Дополнительная информация

### Проверка текущей конфигурации

```powershell
# Посмотреть переменные CMake
cmake -L build/

# Посмотреть компилятор
cmake -L build/ | Select-String "CMAKE_CXX_COMPILER"

# Посмотреть генератор
cmake -L build/ | Select-String "CMAKE_GENERATOR"
```

### Verbose конфигурация

Для отладки проблем с поиском компилятора:

```powershell
cmake -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=Automation/CMAKE/Toolchains/toolchain-clang-cl.cmake `
  -B build `
  --debug-output
```

### Логи CMake

Логи конфигурации находятся в:

- `build/CMakeFiles/CMakeConfigureLog.yaml` - детальный лог поиска инструментов
- `build/CMakeCache.txt` - кэш с найденными путями

## 🔗 Полезные ссылки

- [LLVM Releases](https://github.com/llvm/llvm-project/releases)
- [CMake Documentation - Toolchains](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
- [Clang-CL Documentation](https://clang.llvm.org/docs/MSVCCompatibility.html)

## ✨ Рабочий процесс

Рекомендуемый процесс для работы с проектом:

```powershell
# 1. Диагностика (первый раз или при проблемах)
.\Automation\diagnose_build_environment.ps1

# 2. Если нужно - установить недостающие инструменты

# 3. Очистить кэш (если были проблемы)
.\Automation\clean_cmake_cache.ps1

# 4. Настроить окружение (опционально)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

# 5. Конфигурировать проект
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=Automation/CMAKE/Toolchains/toolchain-clang-cl.cmake -B build

# 6. Собрать
cmake --build build --config Release

# 7. При проблемах - вернуться к шагу 1
```

