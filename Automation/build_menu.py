"""
HuyEngine - Interactive Build Menu
Интерактивное меню для управления сборкой
"""

import os
import sys

try:
    from .build_config import BuildConfig, IDE, BuildSystem, Configuration, Platform, Toolchain, ToolPaths
except ImportError:
    from build_config import BuildConfig, IDE, BuildSystem, Configuration, Platform, Toolchain, ToolPaths


class Colors:
    """ANSI цвета для терминала"""
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

    @staticmethod
    def disable():
        """Отключает цвета (для Windows без поддержки ANSI)"""
        Colors.HEADER = ''
        Colors.OKBLUE = ''
        Colors.OKCYAN = ''
        Colors.OKGREEN = ''
        Colors.WARNING = ''
        Colors.FAIL = ''
        Colors.ENDC = ''
        Colors.BOLD = ''
        Colors.UNDERLINE = ''


# Включаем поддержку ANSI в Windows 10+
if sys.platform == "win32":
    try:
        import ctypes

        kernel32 = ctypes.windll.kernel32
        # Включаем ANSI escape sequences
        kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)  # type: ignore
    except Exception:
        Colors.disable()


class BuildMenu:
    """Интерактивное меню сборки"""

    def __init__(self):
        self.running = True
        self.BACK_TOKEN = "__BACK__"
        ToolPaths.initialize_tools()
        # Загружаем сохранённое состояние, если есть
        BuildConfig.load_state()

    def clear_screen(self):
        """Очищает экран"""
        os.system('cls' if os.name == 'nt' else 'clear')

    def print_header(self):
        """Выводит заголовок"""
        print(f"{Colors.BOLD}{Colors.OKCYAN}{'=' * 70}{Colors.ENDC}")
        print(f"{Colors.BOLD}{Colors.OKCYAN}{'HuyEngine - Build Automation Menu':^70}{Colors.ENDC}")
        print(f"{Colors.BOLD}{Colors.OKCYAN}{'=' * 70}{Colors.ENDC}\n")

    def print_current_config(self):
        """Print current configuration"""
        print(f"{Colors.BOLD}Current configuration:{Colors.ENDC}")
        print(f"  Project name:   {Colors.OKGREEN}{BuildConfig.get_project_name()}{Colors.ENDC}")
        print(f"  C++ standard:   {Colors.OKGREEN}{BuildConfig.get_cxx_standard()}{Colors.ENDC}")
        print(f"  CMake minimum:  {Colors.OKGREEN}{BuildConfig.get_cmake_minimum_version()}{Colors.ENDC}")
        print(f"  IDE:           {Colors.OKGREEN}{BuildConfig.current_ide.value}{Colors.ENDC}")
        print(f"  Build System:  {Colors.OKGREEN}{BuildConfig.current_build_system.value}{Colors.ENDC}")
        print(f"  Configuration: {Colors.OKGREEN}{BuildConfig.current_configuration.value}{Colors.ENDC}")
        print(f"  Platform:      {Colors.OKGREEN}{BuildConfig.current_platform.value}{Colors.ENDC}")
        print(f"  Toolchain:     {Colors.OKGREEN}{BuildConfig.current_toolchain.value}{Colors.ENDC}")
        print(f"  Build Folder:  {Colors.OKGREEN}{BuildConfig.get_build_folder()}{Colors.ENDC}")
        print()

    def print_tools_status(self):
        """Print detected tools status"""
        print(f"{Colors.BOLD}Tools status:{Colors.ENDC}")

        cmake_status = f"{Colors.OKGREEN}✓ Найден{Colors.ENDC}" if BuildConfig.CMAKE_PATH else f"{Colors.FAIL}✗ Не найден{Colors.ENDC}"
        print(f"  CMake:  {cmake_status}")
        if BuildConfig.CMAKE_PATH:
            print(f"          {BuildConfig.CMAKE_PATH}")

        ninja_status = f"{Colors.OKGREEN}✓ Найден{Colors.ENDC}" if BuildConfig.NINJA_PATH else f"{Colors.FAIL}✗ Не найден{Colors.ENDC}"
        print(f"  Ninja:  {ninja_status}")
        if BuildConfig.NINJA_PATH:
            print(f"          {BuildConfig.NINJA_PATH}")

        vs_status = f"{Colors.OKGREEN}✓ Найден{Colors.ENDC}" if BuildConfig.VCVARSALL_PATH else f"{Colors.FAIL}✗ Не найден{Colors.ENDC}"
        print(f"  MSVC:   {vs_status}")
        if BuildConfig.CXX_COMPILER:
            print(f"          {BuildConfig.CXX_COMPILER}")

        clang_status = f"{Colors.OKGREEN}✓ Найден{Colors.ENDC}" if BuildConfig.CLANG_CL_PATH or BuildConfig.CLANGXX_PATH else f"{Colors.FAIL}✗ Не найден{Colors.ENDC}"
        print(f"  LLVM:   {clang_status}")
        if BuildConfig.CLANG_CL_PATH:
            print(f"          clang-cl: {BuildConfig.CLANG_CL_PATH}")
        if BuildConfig.CLANGXX_PATH:
            print(f"          clang++:  {BuildConfig.CLANGXX_PATH}")

        mingw_status = f"{Colors.OKGREEN}✓ Найден{Colors.ENDC}" if BuildConfig.GCC_PATH and BuildConfig.GPP_PATH else f"{Colors.FAIL}✗ Не найден{Colors.ENDC}"
        print(f"  MinGW:  {mingw_status}")
        if BuildConfig.GCC_PATH:
            print(f"          gcc:  {BuildConfig.GCC_PATH}")
        if BuildConfig.GPP_PATH:
            print(f"          g++:  {BuildConfig.GPP_PATH}")
        print()

    def select_from_enum(self, enum_class, prompt):
        """Select value from enum (supports '0. Back')"""
        print(f"\n{Colors.BOLD}{prompt}{Colors.ENDC}")
        options = list(enum_class)
        print("  0. Back")
        for i, option in enumerate(options, 1):
            print(f"  {i}. {option.value}")

        while True:
            try:
                choice = input(f"\n{Colors.OKCYAN}Select option (0-{len(options)}): {Colors.ENDC}").strip()
                if choice == "0":
                    return self.BACK_TOKEN
                if not choice:
                    return None
                idx = int(choice) - 1
                if 0 <= idx < len(options):
                    return options[idx]
                else:
                    print(f"{Colors.FAIL}Invalid choice. Try again.{Colors.ENDC}")
            except ValueError:
                print(f"{Colors.FAIL}Enter a number.{Colors.ENDC}")
            except KeyboardInterrupt:
                return self.BACK_TOKEN

    def menu_configure(self):
        """Configuration menu"""
        self.clear_screen()
        self.print_header()
        print(f"{Colors.BOLD}BUILD CONFIGURATION{Colors.ENDC}\n")

        # Select IDE
        ide = self.select_from_enum(IDE, "Select IDE:")
        if ide == self.BACK_TOKEN:
            return
        if ide:
            BuildConfig.current_ide = ide

        # Select build system
        build_sys = self.select_from_enum(BuildSystem, "Select build system:")
        if build_sys == self.BACK_TOKEN:
            return
        if build_sys:
            BuildConfig.current_build_system = build_sys

        # Select configuration
        config = self.select_from_enum(Configuration, "Select configuration:")
        if config == self.BACK_TOKEN:
            return
        if config:
            BuildConfig.current_configuration = config

        # Select platform
        platform = self.select_from_enum(Platform, "Select platform:")
        if platform == self.BACK_TOKEN:
            return
        if platform:
            BuildConfig.current_platform = platform

        # Select toolchain
        toolchain = self.select_from_enum(Toolchain, "Select toolchain (compiler):")
        if toolchain == self.BACK_TOKEN:
            return
        if toolchain:
            BuildConfig.current_toolchain = toolchain

        # Prompt for project name
        try:
            proj = input(
                f"\n{Colors.OKCYAN}Project name (leave empty to keep '{BuildConfig.PROJECT_NAME}'): {Colors.ENDC}").strip()
            if proj == "0":
                return
            if proj:
                BuildConfig.PROJECT_NAME = proj
        except KeyboardInterrupt:
            return

        # Prompt for C++ standard
        try:
            std_in = input(
                f"\n{Colors.OKCYAN}C++ standard (default {BuildConfig.CXX_STANDARD}, e.g. 17/20/23, or 0 to Back): {Colors.ENDC}").strip()
            if std_in == "0":
                return
            if std_in:
                try:
                    val = int(std_in)
                    BuildConfig.CXX_STANDARD = val
                except ValueError:
                    print(f"{Colors.WARNING}Invalid number, keeping {BuildConfig.CXX_STANDARD}.{Colors.ENDC}")
        except KeyboardInterrupt:
            return

        # Prompt for CMake minimum version
        try:
            cmv = input(
                f"\n{Colors.OKCYAN}CMake minimum required (default {BuildConfig.CMAKE_MINIMUM_VERSION}): {Colors.ENDC}").strip()
            if cmv == "0":
                return
            if cmv:
                BuildConfig.CMAKE_MINIMUM_VERSION = cmv
        except KeyboardInterrupt:
            return

        print(f"\n{Colors.OKGREEN}✓ Configuration updated!{Colors.ENDC}")
        # Сохраняем состояние после обновления
        if BuildConfig.save_state():
            print(f"{Colors.OKGREEN}✓ Configuration saved.{Colors.ENDC}")
        else:
            print(f"{Colors.WARNING}⚠ Failed to save configuration.{Colors.ENDC}")
        input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")

    def menu_build_actions(self):
        """Build actions menu"""
        self.clear_screen()
        self.print_header()
        self.print_current_config()

        print(f"{Colors.BOLD}BUILD ACTIONS{Colors.ENDC}\n")
        print("  1. Generate (Project generation)")
        print("  2. Build (Build project)")
        print("  3. Rebuild (Rebuild project)")
        print("  4. Clean (Clean current folder)")
        print("  5. Clean All (Clean all build folders)")
        print("  6. Run (Run executable)")
        print("  7. Generate VS Solution (Quick)")
        print("  0. Back")

        choice = input(f"\n{Colors.OKCYAN}Select action: {Colors.ENDC}").strip()

        if choice == "1":
            self.action_generate()
        elif choice == "2":
            self.action_build()
        elif choice == "3":
            self.action_rebuild()
        elif choice == "4":
            self.action_clean()
        elif choice == "5":
            self.action_clean_all()
        elif choice == "6":
            self.action_run()
        elif choice == "7":
            self.action_generate_vs_solution()
        elif choice == "0":
            return
        else:
            print(f"{Colors.FAIL}Invalid choice.{Colors.ENDC}")
            input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")

    def action_generate(self):
        """Генерация проекта"""
        print(f"\n{Colors.OKBLUE}→ Запуск генерации проекта...{Colors.ENDC}\n")
        from build_actions import generate_project
        success = generate_project()

        if success:
            print(f"\n{Colors.OKGREEN}✓ Проект успешно сгенерирован!{Colors.ENDC}")
        else:
            print(f"\n{Colors.FAIL}✗ Ошибка генерации проекта.{Colors.ENDC}")

        input(f"\n{Colors.OKCYAN}Нажмите Enter для продолжения...{Colors.ENDC}")

    def action_build(self):
        """Сборка проекта"""
        print(f"\n{Colors.OKBLUE}→ Запуск сборки проекта...{Colors.ENDC}\n")
        from build_actions import build_project
        success = build_project()

        if success:
            print(f"\n{Colors.OKGREEN}✓ Проект успешно собран!{Colors.ENDC}")
        else:
            print(f"\n{Colors.FAIL}✗ Ошибка сборки проекта.{Colors.ENDC}")

        input(f"\n{Colors.OKCYAN}Нажмите Enter для продолжения...{Colors.ENDC}")

    def action_rebuild(self):
        """Пересборка проекта"""
        print(f"\n{Colors.OKBLUE}→ Запуск пересборки проекта...{Colors.ENDC}\n")
        from build_actions import rebuild_project
        success = rebuild_project()

        if success:
            print(f"\n{Colors.OKGREEN}✓ Проект успешно пересобран!{Colors.ENDC}")
        else:
            print(f"\n{Colors.FAIL}✗ Ошибка пересборки проекта.{Colors.ENDC}")

        input(f"\n{Colors.OKCYAN}Нажмите Enter для продолжения...{Colors.ENDC}")

    def action_clean(self):
        """Очистка текущей папки сборки"""
        print(f"\n{Colors.WARNING}→ Удаление папки {BuildConfig.get_build_folder()}...{Colors.ENDC}\n")
        from build_actions import clean_build_folder
        success = clean_build_folder(BuildConfig.get_build_folder())

        if success:
            print(f"\n{Colors.OKGREEN}✓ Build folder deleted!{Colors.ENDC}")
        else:
            print(f"\n{Colors.WARNING}⚠ Build folder does not exist or already deleted.{Colors.ENDC}")

        input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")

    def action_clean_all(self):
        """Очистка всех папок сборки"""
        print(f"\n{Colors.WARNING}→ Удаление всех папок сборки...{Colors.ENDC}\n")
        from build_actions import clean_all_build_folders
        clean_all_build_folders()

        print(f"\n{Colors.OKGREEN}✓ Все папки сборки удалены!{Colors.ENDC}")
        input(f"\n{Colors.OKCYAN}Нажмите Enter для продолжения...{Colors.ENDC}")

    def action_run(self):
        """Run application"""
        print(f"\n{Colors.OKBLUE}→ Running application...{Colors.ENDC}\n")
        from build_actions import run_executable
        run_executable()

        input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")

    def action_generate_vs_solution(self):
        """Быстрая генерация Visual Studio solution"""
        print(f"\n{Colors.OKBLUE}→ Генерация Visual Studio solution...{Colors.ENDC}\n")

        # Сохраняем текущие настройки
        saved_ide = BuildConfig.current_ide
        saved_build_sys = BuildConfig.current_build_system

        # Временно переключаем на VS
        BuildConfig.current_ide = IDE.VISUAL_STUDIO
        BuildConfig.current_build_system = BuildSystem.MSBUILD

        from build_actions import generate_project
        success = generate_project()

        # Восстанавливаем настройки
        BuildConfig.current_ide = saved_ide
        BuildConfig.current_build_system = saved_build_sys

        if success:
            sln_path = BuildConfig.PROJECT_ROOT / "vs-build" / f"{BuildConfig.get_project_name()}.sln"
            print(f"\n{Colors.OKGREEN}✓ Visual Studio solution сгенерирован!{Colors.ENDC}")
            print(f"  Путь: {sln_path}")

            # Предлагаем открыть в Visual Studio
            try:
                open_vs = input(f"\n{Colors.OKCYAN}Открыть в Visual Studio? (y/n): {Colors.ENDC}").strip().lower()
                if open_vs == 'y':
                    import subprocess
                    subprocess.Popen(f'start "" "{sln_path}"', shell=True)
                    print(f"{Colors.OKGREEN}✓ Visual Studio запущен{Colors.ENDC}")
            except Exception as e:
                print(f"{Colors.WARNING}⚠ Не удалось открыть VS: {e}{Colors.ENDC}")
        else:
            print(f"\n{Colors.FAIL}✗ Ошибка генерации Visual Studio solution{Colors.ENDC}")

        input(f"\n{Colors.OKCYAN}Нажмите Enter для продолжения...{Colors.ENDC}")

    def menu_tools(self):
        """Меню инструментов"""
        self.clear_screen()
        self.print_header()
        self.print_tools_status()

        print(f"{Colors.BOLD}ИНСТРУМЕНТЫ{Colors.ENDC}\n")
        print("  1. Clang-Format (Форматирование кода)")
        print("  2. Обновить пути к инструментам")
        print("  0. Назад")

        choice = input(f"\n{Colors.OKCYAN}Выберите действие: {Colors.ENDC}").strip()

        if choice == "1":
            self.tool_clang_format()
        elif choice == "2":
            ToolPaths.initialize_tools()
            print(f"\n{Colors.OKGREEN}✓ Paths updated!{Colors.ENDC}")
            input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")
        elif choice == "0":
            return
        else:
            print(f"{Colors.FAIL}Invalid choice.{Colors.ENDC}")
            input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")

    def tool_clang_format(self):
        """Run clang-format"""
        print(f"\n{Colors.OKBLUE}→ Running clang-format...{Colors.ENDC}\n")
        from build_actions import run_clang_format
        success = run_clang_format()

        if success:
            print(f"\n{Colors.OKGREEN}✓ Formatting completed!{Colors.ENDC}")
        else:
            print(f"\n{Colors.FAIL}✗ Formatting error.{Colors.ENDC}")

        input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")

    def main_menu(self):
        """Main menu"""
        while self.running:
            self.clear_screen()
            self.print_header()
            self.print_current_config()

            print(f"{Colors.BOLD}MAIN MENU{Colors.ENDC}\n")
            print("  1. Configuration")
            print("  2. Build")
            print("  3. Tools")
            print("  0. Exit")

            choice = input(f"\n{Colors.OKCYAN}Select menu item: {Colors.ENDC}").strip()

            if choice == "1":
                self.menu_configure()
            elif choice == "2":
                self.menu_build_actions()
            elif choice == "3":
                self.menu_tools()
            elif choice == "0":
                self.running = False
                print(f"\n{Colors.OKGREEN}Goodbye!{Colors.ENDC}\n")
            else:
                print(f"{Colors.FAIL}Invalid choice. Try again.{Colors.ENDC}")
                input(f"\n{Colors.OKCYAN}Press Enter to continue...{Colors.ENDC}")

    def run(self):
        """Run menu"""
        try:
            self.main_menu()
        except KeyboardInterrupt:
            print(f"\n\n{Colors.WARNING}Interrupted by user.{Colors.ENDC}")
            print(f"{Colors.OKGREEN}Goodbye!{Colors.ENDC}\n")


if __name__ == "__main__":
    menu = BuildMenu()
    menu.run()
