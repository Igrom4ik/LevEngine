"""
HuyEngine - User Configuration (Optional)
Пользовательские настройки (опционально)

Этот файл можно использовать для переопределения настроек по умолчанию.
Создайте копию этого файла как 'user_config.py' и измените под свои нужды.
"""

# Пользовательские пути к инструментам (если автоопределение не работает)
USER_CMAKE_PATH = None  # Например: r"C:\Custom\Path\cmake.exe"
USER_NINJA_PATH = None  # Например: r"C:\Custom\Path\ninja.exe"
USER_VCVARSALL_PATH = None  # Например: r"C:\VS2022\VC\Auxiliary\Build\vcvarsall.bat"

# Предпочтительные настройки по умолчанию
DEFAULT_IDE = "manual"  # vs, clion, vscode, manual
DEFAULT_BUILD_SYSTEM = "ninja"  # ninja, msbuild, make
DEFAULT_CONFIGURATION = "debug"  # debug, release, relwithdebinfo, minsizerel
DEFAULT_PLATFORM = "x64"  # x64, win32, arm64

# Настройки сборки
DEFAULT_VERBOSE = False
DEFAULT_CLEAN_FIRST = True
DEFAULT_PARALLEL_JOBS = 8  # Количество параллельных задач сборки

# Дополнительные директории для clang-format
ADDITIONAL_SOURCE_DIRS = []  # Например: ["Tests", "Samples"]

# Пользовательские CMake флаги
CUSTOM_CMAKE_FLAGS = []  # Например: ["-DENABLE_TESTING=ON", "-DBUILD_SHARED_LIBS=ON"]

# Игнорировать предупреждения при автоопределении инструментов
SUPPRESS_TOOL_WARNINGS = False

# Пользовательские шаблоны папок сборки
CUSTOM_BUILD_FOLDERS = {
    # "ide_name": "folder_name"
    # Например:
    # "custom_ide": "my-custom-build"
}

# Цветовая схема (для продвинутых пользователей)
USE_COLORS = True  # False для отключения цветов в консоли

# Тайм-аут для команд (в секундах)
COMMAND_TIMEOUT = 3600  # 1 час по умолчанию

# Автоматически открыть проект в IDE после генерации
AUTO_OPEN_IDE = {
    "vs": False,  # Открывать Visual Studio после генерации
    "clion": False,  # Открывать CLion после генерации
    "vscode": False,  # Открывать VSCode после генерации
}

# Путь к решению Visual Studio (если используется кастомное имя)
VS_SOLUTION_NAME = None  # Например: "MyEngine.sln"

# Дополнительные аргументы для clang-format
CLANG_FORMAT_ARGS = ["-i", "--style=file"]  # -i = in-place

# Проверка кода перед сборкой
PRE_BUILD_CHECKS = {
    "format": False,  # Автоматически форматировать перед сборкой
    "warnings": True,  # Показывать предупреждения
}

# Постобработка после сборки
POST_BUILD_ACTIONS = {
    "copy_assets": False,  # Копировать ресурсы
    "run_tests": False,  # Запускать тесты
}

# Пути к дополнительным ресурсам
ASSETS_SOURCE_DIR = "Assets"
ASSETS_TARGET_DIR = "bin/Assets"

# Логирование
ENABLE_LOGGING = False
LOG_FILE = "build.log"
LOG_LEVEL = "INFO"  # DEBUG, INFO, WARNING, ERROR

# =============================================================================
# Применение пользовательских настроек
# =============================================================================

def apply_user_config(build_config):
    """
    Применяет пользовательские настройки к основной конфигурации.
    Вызывается автоматически при импорте.
    """

    # Применяем пользовательские пути
    if USER_CMAKE_PATH:
        build_config.CMAKE_PATH = USER_CMAKE_PATH
    if USER_NINJA_PATH:
        build_config.NINJA_PATH = USER_NINJA_PATH
    if USER_VCVARSALL_PATH:
        build_config.VCVARSALL_PATH = USER_VCVARSALL_PATH

    # Применяем настройки по умолчанию
    # (будет переопределено, если указаны аргументы командной строки)

    # Добавляем дополнительные директории для форматирования
    if ADDITIONAL_SOURCE_DIRS:
        build_config.SOURCE_DIRS.extend(ADDITIONAL_SOURCE_DIRS)


# =============================================================================
# Пример использования
# =============================================================================

"""
Чтобы использовать этот файл:

1. Скопируйте user_config.template.py в user_config.py
2. Отредактируйте настройки в user_config.py
3. Система автоматически загрузит ваши настройки при запуске

Пример user_config.py:

# Мои пути к инструментам
USER_CMAKE_PATH = r"D:\Tools\CMake\bin\cmake.exe"
USER_NINJA_PATH = r"D:\Tools\Ninja\ninja.exe"

# Мои предпочтения
DEFAULT_IDE = "vs"
DEFAULT_BUILD_SYSTEM = "msbuild"
DEFAULT_CONFIGURATION = "release"
DEFAULT_PARALLEL_JOBS = 16

# Мои дополнительные CMake флаги
CUSTOM_CMAKE_FLAGS = [
    "-DENABLE_TESTING=ON",
    "-DBUILD_SHARED_LIBS=OFF",
]

# Автоматически форматировать код перед сборкой
PRE_BUILD_CHECKS = {
    "format": True,
    "warnings": True,
}
"""

