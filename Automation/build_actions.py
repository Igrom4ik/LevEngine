"""
HuyEngine - Build Actions
Действия для сборки проекта
"""

import os
import shutil
import subprocess

try:
    from .build_config import BuildConfig, BuildSystem, Toolchain
except ImportError:
    from build_config import BuildConfig, BuildSystem, Toolchain


def setup_msvc_environment():
    """Setup MSVC environment for compilation"""
    if not BuildConfig.VCVARSALL_PATH or not os.path.exists(BuildConfig.VCVARSALL_PATH):
        return os.environ.copy()

    print(f"→ Setting up MSVC environment...")

    platform_arg = BuildConfig.current_platform.value.lower()
    command = f'"{BuildConfig.VCVARSALL_PATH}" {platform_arg} && set'

    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            env = {}
            for line in result.stdout.split('\n'):
                if '=' in line:
                    key, _, value = line.partition('=')
                    env[key.strip()] = value.strip()
            print(f"✓ MSVC environment configured")
            return env
    except subprocess.TimeoutExpired:
        print(f"⚠ Timeout while setting up MSVC environment")
    except Exception as e:
        print(f"⚠ Error setting up environment: {e}")

    return os.environ.copy()


def run_command(command, cwd=None, env=None):
    """Execute a command in the shell"""
    print(f"\n→ Executing: {command}\n")
    try:
        result = subprocess.run(
            command,
            shell=True,
            cwd=cwd,
            env=env,
            text=True
        )
        return result.returncode == 0
    except Exception as e:
        print(f"✗ Command execution error: {e}")
        return False


def get_cmake_generate_command():
    """Build the command for CMake project generation"""
    if not BuildConfig.CMAKE_PATH:
        print("✗ CMake not found!")
        return None

    build_folder = BuildConfig.get_build_folder()
    generator = BuildConfig.get_cmake_generator()

    args = [
        f'"{BuildConfig.CMAKE_PATH}"',
        f'-S "{BuildConfig.PROJECT_ROOT}"',
        f'-B "{build_folder}"',
        f'-G "{generator}"',
        f'-DCMAKE_BUILD_TYPE={BuildConfig.current_configuration.value}',
    ]

    # Toolchain handling for CMake
    tc_file = None
    if BuildConfig.current_build_system == BuildSystem.NINJA:
        tc_file = BuildConfig.get_toolchain_file()
        if tc_file and os.path.exists(tc_file):
            args.append(f'-DCMAKE_TOOLCHAIN_FILE="{tc_file}"')

        # Provide make program path
        if BuildConfig.NINJA_PATH:
            args.append(f'-DCMAKE_MAKE_PROGRAM="{BuildConfig.NINJA_PATH}"')

        # Export project name and C++ standard explicitly so CMakeLists can use them
        args.append(f'-DPROJECT_NAME="{BuildConfig.get_project_name()}"')
        args.append(f'-DPROJECT_CXX_STANDARD={BuildConfig.get_cxx_standard()}')
        args.append(f'-DCMAKE_MINIMUM_REQUIRED={BuildConfig.get_cmake_minimum_version()}')

        # For manual override when no toolchain file is used
        if BuildConfig.current_toolchain == Toolchain.MSVC:
            # Use cl.exe (via env), don't pass compilers explicitly if toolchain file handles it
            if not tc_file:
                if BuildConfig.CXX_COMPILER:
                    args.append(f'-DCMAKE_CXX_COMPILER="{BuildConfig.CXX_COMPILER}"')
                if BuildConfig.C_COMPILER:
                    args.append(f'-DCMAKE_C_COMPILER="{BuildConfig.C_COMPILER}"')
        elif BuildConfig.current_toolchain == Toolchain.CLANG_CL:
            # With clang-cl toolchain we rely on vcvarsall environment
            if not tc_file and BuildConfig.CLANG_CL_PATH:
                args.append(f'-DCMAKE_CXX_COMPILER="{BuildConfig.CLANG_CL_PATH}"')
                args.append(f'-DCMAKE_C_COMPILER="{BuildConfig.CLANG_CL_PATH}"')
            # Tell CMake to use MSVC-like environment when using clang-cl
            args.append('-T clangcl')
        elif BuildConfig.current_toolchain == Toolchain.MINGW_GCC:
            # Prefer toolchain file; also pass explicit compilers if detected to force MinGW
            if tc_file and os.path.exists(tc_file):
                args.append(f'-DCMAKE_TOOLCHAIN_FILE="{tc_file}"')
            # Always prefer explicit compilers when available to avoid MSVC picking
            if BuildConfig.GPP_PATH:
                args.append(f'-DCMAKE_CXX_COMPILER="{BuildConfig.GPP_PATH}"')
            if BuildConfig.GCC_PATH:
                args.append(f'-DCMAKE_C_COMPILER="{BuildConfig.GCC_PATH}"')
            # Ensure Windows MinGW platform
            args.append('-DCMAKE_SYSTEM_NAME=Windows')

    # Add platform for Visual Studio generator
    if BuildConfig.current_build_system == BuildSystem.MSBUILD:
        if BuildConfig.current_platform.value != "x64":
            args.append(f'-A {BuildConfig.current_platform.value}')

    return ' '.join(args)


def get_cmake_build_command():
    """Build the command for CMake project build"""
    if not BuildConfig.CMAKE_PATH:
        print("✗ CMake not found!")
        return None

    build_folder = BuildConfig.get_build_folder()

    args = [
        f'"{BuildConfig.CMAKE_PATH}"',
        f'--build "{build_folder}"',
        f'--config {BuildConfig.current_configuration.value}',
    ]

    if BuildConfig.clean_first:
        args.append('--clean-first')

    if BuildConfig.verbose:
        args.append('--verbose')

    # Parallel build
    args.append('-j 8')

    return ' '.join(args)


def clean_build_folder(folder_name):
    """Delete the specified build folder"""
    folder_path = BuildConfig.PROJECT_ROOT / folder_name
    if folder_path.exists():
        try:
            shutil.rmtree(folder_path)
            print(f"✓ Folder deleted: {folder_name}")
            return True
        except Exception as e:
            print(f"✗ Error deleting {folder_name}: {e}")
            return False
    else:
        print(f"⚠ Folder {folder_name} does not exist")
        return False


def clean_all_build_folders():
    """Delete all build folders"""
    build_folders = [
        "build",
        "vs-build",
        "build-msvc",
        "build-clang-cl",
        "build-mingw",
        "cmake-build-debug",
        "cmake-build-release",
        "cmake-build-relwithdebinfo",
        "cmake-build-minsizerel",
    ]

    for folder in build_folders:
        clean_build_folder(folder)


def generate_project():
    """Генерирует проект CMake"""
    print("=" * 70)
    print(f"ГЕНЕРАЦИЯ ПРОЕКТА ({BuildConfig.current_configuration.value})")
    print("=" * 70)
    print()

    # Получаем команду
    command = get_cmake_generate_command()
    if not command:
        return False

    # Создаем папку сборки если нужно
    build_folder = BuildConfig.PROJECT_ROOT / BuildConfig.get_build_folder()
    if not build_folder.exists():
        build_folder.mkdir(parents=True)
        print(f"✓ Создана папка: {BuildConfig.get_build_folder()}")

    # Настраиваем окружение
    env = os.environ.copy()

    # If using MSVC or clang-cl, set up MSVC env (libraries, include paths)
    if BuildConfig.current_build_system == BuildSystem.NINJA and \
            BuildConfig.current_toolchain in (Toolchain.MSVC, Toolchain.CLANG_CL) and \
            BuildConfig.VCVARSALL_PATH:
        env = setup_msvc_environment()

    # Выполняем генерацию
    success = run_command(command, cwd=BuildConfig.PROJECT_ROOT, env=env)

    if success:
        print(f"\n✓ Project generated: {BuildConfig.get_build_folder()}/")
        print(f"  Generator: {BuildConfig.get_cmake_generator()}")
        print(f"  Configuration: {BuildConfig.current_configuration.value}")
        print(f"  Toolchain: {BuildConfig.current_toolchain.value}")
    else:
        print(f"\n✗ Ошибка генерации проекта")

    return success


def build_project():
    """Собирает проект CMake"""
    print("=" * 70)
    print(f"СБОРКА ПРОЕКТА ({BuildConfig.current_configuration.value})")
    print("=" * 70)
    print()

    # Проверяем наличие папки сборки
    build_folder = BuildConfig.PROJECT_ROOT / BuildConfig.get_build_folder()
    if not build_folder.exists():
        print(f"✗ Папка сборки не существует: {BuildConfig.get_build_folder()}")
        print(f"  Сначала выполните генерацию проекта!")
        return False

    # Получаем команду
    command = get_cmake_build_command()
    if not command:
        return False

    # Настраиваем окружение
    env = os.environ.copy()
    if BuildConfig.current_build_system == BuildSystem.NINJA and \
            BuildConfig.current_toolchain in (Toolchain.MSVC, Toolchain.CLANG_CL) and \
            BuildConfig.VCVARSALL_PATH:
        env = setup_msvc_environment()

    # Выполняем сборку
    success = run_command(command, cwd=BuildConfig.PROJECT_ROOT, env=env)

    if success:
        exe_path = build_folder / "bin" / BuildConfig.get_executable_name()
        print(f"\n✓ Проект собран успешно!")
        print(f"  Конфигурация: {BuildConfig.current_configuration.value}")
        if exe_path.exists():
            print(f"  Исполняемый файл: {exe_path}")
    else:
        print(f"\n✗ Ошибка сборки проекта")

    return success


def rebuild_project():
    """Пересобирает проект (clean + generate + build)"""
    print("=" * 70)
    print(f"ПЕРЕСБОРКА ПРОЕКТА ({BuildConfig.current_configuration.value})")
    print("=" * 70)
    print()

    # Очищаем
    clean_build_folder(BuildConfig.get_build_folder())

    # Генерируем
    if not generate_project():
        return False

    print()

    # Собираем
    return build_project()


def run_executable():
    """Запускает исполняемый файл"""
    build_folder = BuildConfig.PROJECT_ROOT / BuildConfig.get_build_folder()
    exe_path = build_folder / "bin" / BuildConfig.get_executable_name()

    if not exe_path.exists():
        print(f"✗ Executable not found: {exe_path}")
        print(f"  Build the project first!")
        return False

    print("=" * 70)
    print(f"ЗАПУСК {BuildConfig.EXECUTABLE_NAME}")
    print("=" * 70)
    print()

    return run_command(f'"{exe_path}"', cwd=BuildConfig.PROJECT_ROOT)


def get_source_files(source_dirs, extensions):
    """Собирает список файлов с заданными расширениями"""
    source_files = []

    for source_dir in source_dirs:
        path = BuildConfig.PROJECT_ROOT / source_dir

        if path.is_file():
            # Если это файл
            if any(str(path).endswith(ext) for ext in extensions):
                source_files.append(str(path))
        elif path.is_dir():
            # Если это папка, ищем рекурсивно
            for root, _, files in os.walk(path):
                for file in files:
                    if any(file.endswith(ext) for ext in extensions):
                        source_files.append(os.path.join(root, file))

    return source_files


def run_clang_format():
    """Apply clang-format to source files"""
    print("=" * 70)
    print("CODE FORMATTING (CLANG-FORMAT)")
    print("=" * 70)
    print()

    extensions = ['.cpp', '.h', '.hpp', '.c']
    format_sources = get_source_files(BuildConfig.SOURCE_DIRS, extensions)

    if not format_sources:
        print(f'✗ No files to format found in {BuildConfig.SOURCE_DIRS}')
        return False

    print(f"Files found: {len(format_sources)}")
    for f in format_sources[:15]:  # Show first 15
        print(f"  - {f}")
    if len(format_sources) > 15:
        print(f"  ... and {len(format_sources) - 15} more")
    print()

    # Check if clang-format is available
    try:
        result = subprocess.run(["clang-format", "--version"], capture_output=True, text=True)
        if result.returncode != 0:
            print("✗ clang-format not found in the system!")
            print("  Install clang-format and add it to PATH")
            return False
    except FileNotFoundError:
        print("✗ clang-format not found in the system!")
        print("  Install clang-format and add it to PATH")
        return False

    # Apply formatting
    command = ['clang-format', '-i'] + format_sources
    success = run_command(' '.join(f'"{f}"' if ' ' in f else f for f in command), cwd=BuildConfig.PROJECT_ROOT)

    if success:
        print('\n✓ Formatting completed successfully!')
    else:
        print('\n✗ Formatting error')

    return success
