# 📋 Шпаргалка: Процесс выбора конфигурации

## ⚡ Кратко: что происходит при выборе конфигурации

```
ВЫ ВЫБИРАЕТЕ          →    ЧТО ПРОИСХОДИТ             →    РЕЗУЛЬТАТ
════════════════           ══════════════════             ════════════

IDE: CLion            →    Определяется папка          →  cmake-build-debug/
Configuration: Debug       сборки по правилам               

Build System: Ninja   →    CMake генерирует            →  build.ninja
                          файлы для Ninja                  

Toolchain: MSVC       →    Настраивается               →  Компиляция через cl.exe
                          компилятор MSVC                  со флагами /Od /Zi

Platform: x64         →    Настраивается целевая       →  64-битный exe файл
                          архитектура

                      →    Запускается генерация       →  build-msvc/bin/
                          и сборка проекта                 HuyEngine.exe
```

---

## 🎯 5 параметров конфигурации

### 1️⃣ **IDE** - Где вы работаете

- `Visual Studio` → папка `vs-build/`
- `CLion` → папка `cmake-build-{config}/`
- `Manual` → папка `build-{toolchain}/`

### 2️⃣ **Build System** - Как собирать

- `Ninja` → быстрая параллельная сборка (рекомендуется)
- `MSBuild` → Visual Studio решения (.sln)
- `Make` → Unix-стиль Makefile

### 3️⃣ **Configuration** - Режим сборки

- `Debug` → без оптимизации, полная отладка
- `Release` → максимальная оптимизация
- `RelWithDebInfo` → оптимизация + отладка
- `MinSizeRel` → минимальный размер

### 4️⃣ **Platform** - Архитектура

- `x64` → 64-битный (рекомендуется)
- `Win32` → 32-битный
- `ARM64` → ARM процессоры

### 5️⃣ **Toolchain** - Компилятор

- `MSVC` → Microsoft cl.exe
- `Clang-CL` → LLVM clang-cl.exe (совместим с MSVC)
- `MinGW GCC` → GNU g++.exe (открытый)

---

## 🔄 Что происходит после выбора

### Шаг 1: Сохранение

```python
BuildConfig.current_ide = IDE.CLION
BuildConfig.current_build_system = BuildSystem.NINJA
BuildConfig.current_configuration = Configuration.DEBUG
BuildConfig.current_platform = Platform.X64
BuildConfig.current_toolchain = Toolchain.MSVC
```

### Шаг 2: Определение папки

```python
def get_build_folder():
    if current_ide == CLION and current_configuration == DEBUG:
        return "cmake-build-debug"
    elif current_toolchain == MSVC:
        return "build-msvc"
    # ...
```

### Шаг 3: Генерация команды CMake

```bash
cmake -S "C:/DEVOPS/HuyEngine" \
      -B "build-msvc" \
      -G "Ninja" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_TOOLCHAIN_FILE="toolchain-msvc.cmake"
```

### Шаг 4: Настройка окружения

```bash
# Для MSVC/Clang-CL:
vcvarsall.bat x64

# Устанавливает:
# - PATH к компилятору
# - INCLUDE к заголовочным файлам
# - LIB к библиотекам
```

### Шаг 5: Выполнение

```bash
cmake --build "build-msvc" --config Debug -j 8
```

---

## 📁 Структура папок после генерации

```
HuyEngine/
│
├── build-msvc/              ← MSVC сборка
│   ├── bin/
│   │   └── HuyEngine.exe
│   ├── build.ninja
│   └── CMakeCache.txt
│
├── build-clang-cl/          ← Clang-CL сборка
│   ├── bin/
│   │   └── HuyEngine.exe
│   └── ...
│
├── build-mingw/             ← MinGW сборка
│   ├── bin/
│   │   └── HuyEngine.exe
│   └── ...
│
├── cmake-build-debug/       ← CLion Debug
│   └── ...
│
└── cmake-build-release/     ← CLion Release
    └── ...

Каждая папка полностью независима!
```

---

## 🚀 Быстрые команды

### Интерактивное меню

```bash
python automation_new.py
# Выбираете все параметры в меню
```

### Командная строка

```bash
# MSVC Debug
python automation_new.py -t msvc -c debug generate

# Clang-CL Release
python automation_new.py -t clang-cl -c release build

# MinGW Debug (полная)
python automation_new.py -i manual -b ninja -c debug -t mingw rebuild
```

---

## ✅ Проверка: как узнать текущую конфигурацию

```bash
python automation_new.py
# В меню сразу показывается:

Current configuration:
  IDE:           Manual (Command Line)
  Build System:  Ninja
  Configuration: Debug
  Platform:      x64
  Toolchain:     MSVC (cl)
  Build Folder:  build-msvc
```

---

## 💡 Часто задаваемые вопросы

### ❓ Можно ли иметь несколько конфигураций одновременно?

✅ **Да!** Каждая комбинация IDE/Toolchain создает отдельную папку.

### ❓ Что если я изменю toolchain?

✅ Создастся **новая папка** сборки. Старая останется нетронутой.

### ❓ Нужно ли очищать при переключении?

❌ **Нет!** Разные папки полностью изолированы.

### ❓ Какой toolchain выбрать?

💡 **Рекомендации:**

- **MSVC** - стандарт для Windows, лучшая совместимость
- **Clang-CL** - современный LLVM, лучшие предупреждения
- **MinGW** - открытый, кроссплатформенный

---

## 🎓 Практические примеры

### Пример 1: Разработка в CLion

```
1. Открываете меню
2. Configuration → IDE: CLion
3. Configuration → Build System: Ninja
4. Configuration → Configuration: Debug
5. Build → Generate
6. Build → Build

Результат: cmake-build-debug/bin/HuyEngine.exe
```

### Пример 2: Сравнение компиляторов

```bash
# Собираем MSVC
python automation_new.py -t msvc -c release build

# Собираем Clang-CL
python automation_new.py -t clang-cl -c release build

# Сравниваем размер exe:
dir build-msvc\bin\HuyEngine.exe
dir build-clang-cl\bin\HuyEngine.exe
```

### Пример 3: Отладочная и релизная версия

```bash
# Debug версия
python automation_new.py -c debug build

# Release версия (в другой папке!)
python automation_new.py -c release build

# Обе версии доступны одновременно
```

---

## 📊 Матрица рекомендаций

| Задача                   | IDE    | Build System | Config        | Toolchain |
|--------------------------|--------|--------------|---------------|-----------|
| Быстрая разработка       | CLion  | Ninja        | Debug         | MSVC      |
| Финальная сборка         | Manual | Ninja        | Release       | MSVC      |
| Кроссплатформенность     | Manual | Ninja        | Release       | MinGW     |
| Максимальная диагностика | Manual | Ninja        | Debug         | Clang-CL  |
| Visual Studio проект     | VS     | MSBuild      | Debug/Release | MSVC      |

---

## 🔗 Связанные документы

- **CONFIGURATION_FLOW.md** - Полное описание процесса
- **CONFIGURATION_DIAGRAM.md** - Визуальные диаграммы
- **BUILD_SYSTEM_README.md** - Руководство по системе сборки
- **BUILD_EXAMPLES.md** - Практические примеры

---

**💡 Совет:** Начните с `Manual + Ninja + Debug + MSVC` - это самая универсальная конфигурация!

**Дата создания:** 2025-11-13

