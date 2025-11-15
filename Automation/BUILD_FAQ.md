# HuyEngine - FAQ по системе сборки

## ❓ Часто задаваемые вопросы

### Установка и настройка

#### Q: Что нужно установить для работы?

**A:** Минимальные требования:

- Python 3.6+
- CMake 3.31.6+
- Компилятор C++ (Visual Studio 2019/2022 для Windows)

Рекомендуется также установить Ninja для ускорения сборки.

#### Q: Как установить все зависимости быстро?

**A:** Используйте chocolatey (Windows):

```batch
choco install python cmake ninja
```

Или скачайте вручную:

- CMake: https://cmake.org/download/
- Python: https://www.python.org/downloads/
- Visual Studio: https://visualstudio.microsoft.com/

#### Q: Система не находит CMake/Ninja/Visual Studio. Что делать?

**A:** Есть несколько вариантов:

1. **Добавьте в PATH** (рекомендуется):
    - Откройте "Система" → "Дополнительные параметры" → "Переменные среды"
    - Добавьте пути к CMake и Ninja в PATH

2. **Создайте user_config.py**:
   ```batch
   copy Automation\user_config.template.py Automation\user_config.py
   ```

   Отредактируйте пути в файле:
   ```python
   USER_CMAKE_PATH = r"C:\Path\To\cmake.exe"
   USER_NINJA_PATH = r"C:\Path\To\ninja.exe"
   ```

### Использование

#### Q: Как запустить интерактивное меню?

**A:** Просто выполните:

```batch
build.bat
```

#### Q: Как быстро собрать проект?

**A:** Для первой сборки:

```batch
build.bat rebuild
```

Для последующих сборок:

```batch
build.bat build
```

#### Q: В чем разница между generate, build и rebuild?

**A:**

- **generate** - Генерирует файлы проекта CMake (конфигурация)
- **build** - Собирает проект (компиляция и линковка)
- **rebuild** - Очистка + генерация + сборка (полная пересборка)

Обычно используйте:

- `generate` - при первой настройке или изменении CMakeLists.txt
- `build` - для повседневной разработки
- `rebuild` - когда что-то сломалось или нужна чистая сборка

#### Q: Какую систему сборки выбрать: Ninja или MSBuild?

**A:**

**Ninja** (рекомендуется):

- ✅ Быстрее в 2-3 раза
- ✅ Лучше для командной строки
- ✅ Кросс-платформенный
- ❌ Не интегрируется с Visual Studio IDE

**MSBuild**:

- ✅ Встроен в Visual Studio
- ✅ Можно открыть .sln в Visual Studio
- ✅ Отладка в Visual Studio
- ❌ Медленнее
- ❌ Только для Windows

**Рекомендация**: Используйте Ninja для быстрой сборки в командной строке, MSBuild если работаете в Visual Studio IDE.

#### Q: Как открыть проект в Visual Studio?

**A:**

```batch
# 1. Генерируйте проект для Visual Studio
build.bat generate --ide vs --build-system msbuild

# 2. Откройте solution файл
start vs-build\HuyEngine.sln
```

#### Q: Как собрать для CLion?

**A:**

```batch
# Генерация для CLion
build.bat generate --ide clion --config debug

# CLion автоматически использует cmake-build-* папки
# Просто откройте проект в CLion
```

### Проблемы

#### Q: Ошибка "CMake Error: CMake was unable to find a build program"

**A:** Ninja не найден. Установите Ninja или используйте MSBuild:

```batch
# Установить Ninja
choco install ninja

# Или использовать MSBuild
build.bat generate --build-system msbuild
```

#### Q: Ошибка "No CMAKE_CXX_COMPILER could be found"

**A:** Компилятор не найден. Установите Visual Studio с компонентами C++:

1. Запустите Visual Studio Installer
2. Выберите "Разработка классических приложений на C++"
3. Установите

#### Q: Сборка завершается с ошибками компиляции

**A:** Попробуйте полную пересборку:

```batch
build.bat clean-all
build.bat rebuild
```

Если не помогает, проверьте:

- Правильность кода в ваших файлах
- Версию компилятора (нужен C++17 или выше)
- Логи с подробным выводом: `build.bat --verbose build`

#### Q: "Python is not installed" или "Python не найден"

**A:** Установите Python:

```batch
# Через chocolatey
choco install python

# Или скачайте с python.org
# Убедитесь что Python добавлен в PATH
```

#### Q: Кириллица отображается неправильно

**A:** Это нормально для некоторых версий Windows. Система работает правильно, просто текст отображается в неправильной
кодировке. Функциональность не нарушена.

Для исправления:

1. Используйте Windows Terminal вместо cmd.exe
2. Или отключите цвета в user_config.py: `USE_COLORS = False`

#### Q: Долго выполняется настройка MSVC окружения

**A:** Это нормально при первом запуске с Ninja. Система настраивает окружение Visual Studio. Последующие запуски будут
быстрее.

Чтобы ускорить, используйте MSBuild:

```batch
build.bat -b msbuild build
```

### Папки и файлы

#### Q: Где находится исполняемый файл после сборки?

**A:** В зависимости от конфигурации:

- Ninja/Manual: `build/bin/HuyEngine.exe`
- CLion Debug: `cmake-build-debug/bin/HuyEngine.exe`
- CLion Release: `cmake-build-release/bin/HuyEngine.exe`
- Visual Studio: `vs-build/bin/HuyEngine.exe`

#### Q: Можно ли удалить папки сборки?

**A:** Да, безопасно. Используйте:

```batch
# Удалить текущую папку
build.bat clean

# Удалить все папки сборки
build.bat clean-all
```

Затем просто пересоздайте:

```batch
build.bat rebuild
```

#### Q: Зачем так много папок сборки?

**A:** Каждая конфигурация (Debug/Release) и IDE (CLion/VS) использует свою папку. Это позволяет:

- Хранить несколько конфигураций одновременно
- Быстро переключаться между ними
- Не пересобирать все при смене конфигурации

#### Q: Какие папки добавить в .gitignore?

**A:** Уже добавлено в `.gitignore`:

```
build/
cmake-build-*/
vs-build/
Automation/user_config.py
build.log
```

### Продвинутое использование

#### Q: Как добавить свои CMake флаги?

**A:** Создайте `user_config.py`:

```python
CUSTOM_CMAKE_FLAGS = [
    "-DENABLE_TESTING=ON",
    "-DBUILD_SHARED_LIBS=OFF",
    "-DCUSTOM_OPTION=VALUE"
]
```

#### Q: Как изменить количество параллельных задач сборки?

**A:** В `user_config.py`:

```python
DEFAULT_PARALLEL_JOBS = 16  # Вместо 8 по умолчанию
```

#### Q: Можно ли автоматически форматировать код перед сборкой?

**A:** Да, в `user_config.py`:

```python
PRE_BUILD_CHECKS = {
    "format": True,
    "warnings": True,
}
```

#### Q: Как интегрировать в CI/CD?

**A:** Используйте режим командной строки:

```yaml
# GitHub Actions пример
- name: Build
  run: |
    python Automation/automation_new.py generate --config release
    python Automation/automation_new.py build --config release
```

См. `BUILD_EXAMPLES.md` для полных примеров.

#### Q: Можно ли использовать другой компилятор (не MSVC)?

**A:** Да, установите нужный компилятор и укажите в `user_config.py`:

```python
USER_CXX_COMPILER = r"C:\Path\To\clang++.exe"
USER_C_COMPILER = r"C:\Path\To\clang.exe"
```

Или укажите через переменные окружения CMake:

```batch
set CMAKE_CXX_COMPILER=clang++
build.bat generate
```

### Разное

#### Q: Как обновить систему сборки до новой версии?

**A:** Просто обновите файлы из репозитория:

```batch
git pull
```

Ваша конфигурация в `user_config.py` сохранится (она в .gitignore).

#### Q: Где найти примеры использования?

**A:** Смотрите документацию:

- `BUILD_QUICKSTART.md` - Быстрый старт
- `BUILD_SYSTEM_README.md` - Полная документация
- `BUILD_EXAMPLES.md` - Примеры для различных сценариев
- `BUILD_FAQ.md` - Этот файл

#### Q: Как вернуться к старой системе сборки?

**A:** Старая система сохранена:

```batch
# Старый способ
cd Automation
python automation.py build --configuration Release

# Или используйте старый build.bat
cd Automation
build.bat build Release
```

#### Q: В чем преимущества новой системы?

**A:**

- ✅ Интерактивное меню
- ✅ Поддержка разных IDE
- ✅ Выбор системы сборки
- ✅ Отдельные папки для конфигураций
- ✅ Автоопределение инструментов
- ✅ Цветной вывод
- ✅ Лучшая документация
- ✅ Гибкая настройка через user_config.py

#### Q: Система не работает на моем компьютере. Где получить помощь?

**A:** Проверьте:

1. Установлены ли все зависимости (Python, CMake, компилятор)
2. Добавлены ли они в PATH
3. Запустите с подробным выводом: `build.bat --verbose build`
4. Проверьте логи: они выводятся в консоль
5. Создайте issue в репозитории с описанием проблемы и логами

#### Q: Можно ли использовать систему на Linux/macOS?

**A:** Да, с небольшими изменениями:

Linux/macOS:

```bash
# Вместо build.bat используйте напрямую Python
python3 Automation/automation_new.py

# Или создайте build.sh:
#!/bin/bash
python3 Automation/automation_new.py "$@"
chmod +x build.sh
./build.sh
```

Большинство функций работают кросс-платформенно. MSBuild доступен только на Windows.

---

**Не нашли ответ?**

Создайте issue в репозитории или обратитесь к полной документации в `BUILD_SYSTEM_README.md`.

**Последнее обновление**: 2025-01-13  
**Версия**: 1.0

