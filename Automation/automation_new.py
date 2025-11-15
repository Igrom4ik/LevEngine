"""
HuyEngine - Build Automation (New Version)
Enhanced build automation system with interactive menu support
"""

import argparse
import io
import sys

# Fix Unicode output for Windows console
if sys.platform == "win32":
    try:
        # Set UTF-8 encoding for stdout and stderr
        sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
        sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')
    except Exception:
        pass

try:
    from .build_config import BuildConfig, IDE, BuildSystem, Configuration, Platform, Toolchain, ToolPaths
    from .build_actions import (
        generate_project, build_project, rebuild_project,
        clean_build_folder, clean_all_build_folders,
        run_executable, run_clang_format
    )
    from .build_menu import BuildMenu
except ImportError:
    from build_config import BuildConfig, IDE, BuildSystem, Configuration, Platform, Toolchain, ToolPaths
    from build_actions import (
        generate_project, build_project, rebuild_project,
        clean_build_folder, clean_all_build_folders,
        run_executable, run_clang_format
    )
    from build_menu import BuildMenu


def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(
        description="HuyEngine - Build Automation System",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Usage examples:

  Interactive mode:
    python automation_new.py

  Command line:
    python automation_new.py generate --ide clion --config Debug
    python automation_new.py build --build-system ninja --config Release
    python automation_new.py rebuild --ide vs --build-system msbuild
    python automation_new.py clean
    python automation_new.py clean-all
    python automation_new.py run
    python automation_new.py format

  Quick commands:
    python automation_new.py -i clion -c Debug generate
    python automation_new.py -b ninja build
        """
    )

    # Main action
    parser.add_argument(
        "action",
        nargs='?',
        choices=[
            'generate', 'build', 'rebuild', 'clean', 'clean-all', 'run', 'format', 'menu'
        ],
        default='menu',
        help="Action to perform (default: menu)"
    )

    # IDE
    parser.add_argument(
        '-i', '--ide',
        type=str,
        choices=['vs', 'visual-studio', 'clion', 'vscode', 'manual'],
        help="IDE selection"
    )

    # Build system
    parser.add_argument(
        '-b', '--build-system',
        type=str,
        choices=['ninja', 'msbuild', 'make'],
        help="Build system"
    )

    # Configuration
    parser.add_argument(
        '-c', '--config',
        type=str,
        choices=['debug', 'release', 'relwithdebinfo', 'minsizerel'],
        help="Build configuration"
    )

    # Platform
    parser.add_argument(
        '-p', '--platform',
        type=str,
        choices=['x64', 'win32', 'arm64'],
        help="Target platform"
    )

    # Toolchain
    parser.add_argument(
        '-t', '--toolchain',
        type=str,
        choices=['msvc', 'clang-cl', 'mingw'],
        help="Compiler toolchain"
    )

    # Options
    parser.add_argument(
        '--verbose',
        action='store_true',
        help="Verbose output"
    )

    parser.add_argument(
        '--no-clean',
        action='store_true',
        help="Don't clean before build"
    )

    return parser.parse_args()


def apply_arguments(args):
    """Apply arguments to configuration"""

    # IDE
    if args.ide:
        ide_map = {
            'vs': IDE.VISUAL_STUDIO,
            'visual-studio': IDE.VISUAL_STUDIO,
            'clion': IDE.CLION,
            'vscode': IDE.VSCODE,
            'manual': IDE.MANUAL
        }
        BuildConfig.current_ide = ide_map[args.ide]

    # Build system
    if args.build_system:
        bs_map = {
            'ninja': BuildSystem.NINJA,
            'msbuild': BuildSystem.MSBUILD,
            'make': BuildSystem.MAKE
        }
        BuildConfig.current_build_system = bs_map[args.build_system]

    # Configuration
    if args.config:
        config_map = {
            'debug': Configuration.DEBUG,
            'release': Configuration.RELEASE,
            'relwithdebinfo': Configuration.RELWITHDEBINFO,
            'minsizerel': Configuration.MINSIZEREL
        }
        BuildConfig.current_configuration = config_map[args.config]

    # Platform
    if args.platform:
        platform_map = {
            'x64': Platform.X64,
            'win32': Platform.WIN32,
            'arm64': Platform.ARM64
        }
        BuildConfig.current_platform = platform_map[args.platform]

    # Toolchain
    if args.toolchain:
        tc_map = {
            'msvc': Toolchain.MSVC,
            'clang-cl': Toolchain.CLANG_CL,
            'mingw': Toolchain.MINGW_GCC,
        }
        BuildConfig.current_toolchain = tc_map[args.toolchain]

    # Options
    if args.verbose:
        BuildConfig.verbose = True

    if args.no_clean:
        BuildConfig.clean_first = False


def print_config():
    """Print current configuration"""
    print("\n" + "=" * 70)
    print("HUYENGINE BUILD AUTOMATION")
    print("=" * 70)
    print(f"\nConfiguration:")
    print(f"  IDE:           {BuildConfig.current_ide.value}")
    print(f"  Build System:  {BuildConfig.current_build_system.value}")
    print(f"  Configuration: {BuildConfig.current_configuration.value}")
    print(f"  Platform:      {BuildConfig.current_platform.value}")
    print(f"  Toolchain:     {BuildConfig.current_toolchain.value}")
    print(f"  Build Folder:  {BuildConfig.get_build_folder()}")
    print()


def execute_action(action):
    """Execute the specified action"""
    actions = {
        'generate': generate_project,
        'build': build_project,
        'rebuild': rebuild_project,
        'clean': lambda: clean_build_folder(BuildConfig.get_build_folder()),
        'clean-all': clean_all_build_folders,
        'run': run_executable,
        'format': run_clang_format,
    }

    if action in actions:
        print_config()
        success = actions[action]()
        return 0 if success else 1
    else:
        print(f"✗ Unknown action: {action}")
        return 1


def main():
    """Main function"""
    # Initialize tools
    ToolPaths.initialize_tools()

    # Parse arguments
    args = parse_arguments()

    # Apply arguments
    apply_arguments(args)

    # Execute action
    if args.action == 'menu':
        # Interactive mode
        menu = BuildMenu()
        menu.run()
        return 0
    else:
        # Command line mode
        return execute_action(args.action)


if __name__ == "__main__":
    try:
        exit_code = main()
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print("\n\n⚠ Interrupted by user.")
        sys.exit(130)
    except Exception as e:
        print(f"\n✗ Critical error: {e}")
        import traceback

        traceback.print_exc()
        sys.exit(1)
