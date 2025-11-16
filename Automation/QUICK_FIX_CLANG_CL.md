# Быстрое решение: CMake не находит clang-cl

## 🚨 Проблема

CMake не может найти компилятор и падает с ошибкой:

- `CMAKE_CXX_COMPILER-NOTFOUND`
- `clang-cl compiler not found`
- CMake находит g++ из ucrt64 вместо clang-cl

## ✅ Быстрое решение

### 1. Проверьте наличие clang-cl

```powershell
Get-Command clang-cl -ErrorAction SilentlyContinue
```

### 2. Если clang-cl НЕ найден - установите:

**Вариант А: LLVM (быстрее)**

1. Скачайте: https://github.com/llvm/llvm-project/releases/latest
2. Найдите файл `LLVM-XX.X.X-win64.exe`
3. Установите (по умолчанию в `C:\Program Files\LLVM`)
4. Убедитесь, что опция "Add LLVM to PATH" отмечена

**Вариант Б: Visual Studio**

1. Откройте Visual Studio Installer
2. Нажмите "Изменить" для VS 2022
3. Перейдите в "Отдельные компоненты"
4. Найдите и отметьте: **"C++ Clang tools for Windows"**
5. Установите

### 3. Очистите кэш CMake

```powershell
cd C:\DEVOPS\HuyEngine
.\Automation\clean_cmake_cache.ps1 -BuildDir build
```

### 4. Переконфигурируйте проект

```powershell
cmake -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=Automation/CMAKE/Toolchains/toolchain-clang-cl.cmake `
  -DCMAKE_BUILD_TYPE=Release `
  -B build
```

### 5. Соберите

```powershell
cmake --build build --config Release
```

## 🔍 Если всё ещё не работает

### Используйте диагностику:

```powershell
.\Automation\diagnose_build_environment.ps1
```

Это покажет:

- Найден ли clang-cl
- Есть ли конфликты с MSYS2/MinGW
- Состояние кэша CMake
- Подробные рекомендации

## 📝 Что было исправлено

### Обновлён toolchain-clang-cl.cmake:

- ✅ Автоматически ищет clang-cl в стандартных местах
- ✅ Даёт понятные ошибки с инструкциями
- ✅ Поддерживает LLVM и Visual Studio установки

### Созданы инструменты:

- ✅ `diagnose_build_environment.ps1` - диагностика окружения
- ✅ `clean_cmake_cache.ps1` - очистка кэша CMake

## 💡 Дополнительные советы

### Если в PATH есть MSYS2/ucrt64:

Временно удалите для этой сессии:

```powershell
$env:PATH = ($env:PATH -split ';' | Where-Object {
    $_ -notlike "*msys*" -and $_ -notlike "*ucrt64*"
}) -join ';'
```

### Для гарантии работы с MSVC окружением:

```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```

### Проверка после исправления:

```powershell
# Проверить, что clang-cl найден
Get-Command clang-cl

# Проверить конфигурацию CMake
cmake -L build/ | Select-String "CMAKE_CXX_COMPILER"

# Убедиться, что нет g++ из ucrt64
Get-Command g++ -ErrorAction SilentlyContinue
```

## 📚 Полная документация

Смотрите файл `CMAKE_TOOLS_README.md` для подробной информации об инструментах.

