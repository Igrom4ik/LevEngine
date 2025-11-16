"""
User config for HuyEngine automation (copy of template with examples).
Edit the values below to match your machine and save as Automation/user_config.py.
This file will be loaded automatically by the automation scripts.
"""

# --- Tool paths (set these if auto-detection fails) ---
# Set absolute paths to cmake.exe and vcvarsall.bat on your machine.
# If you don't know the path, try the commands described in the README or run
# `where cmake` / `where vswhere` from PowerShell to locate them.

# Example (uncomment and edit if needed):
# USER_CMAKE_PATH = r"C:\Program Files\CMake\bin\cmake.exe"
# USER_VCVARSALL_PATH = r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"

# If Visual Studio 2019 installed:
# USER_VCVARSALL_PATH = r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"

# By default keep None (auto-detection will run):
USER_CMAKE_PATH = None
USER_NINJA_PATH = None
USER_VCVARSALL_PATH = None

# Preferred defaults (optional):
DEFAULT_IDE = "vs"           # vs, clion, vscode, manual
DEFAULT_BUILD_SYSTEM = "msbuild"  # ninja, msbuild, make
DEFAULT_CONFIGURATION = "debug"   # debug, release, relwithdebinfo, minsizerel
DEFAULT_PLATFORM = "x64"

# Suppress automation warnings about missing tools (if you want cleaner output)
SUPPRESS_TOOL_WARNINGS = False

# Parallel jobs for build (optional)
DEFAULT_PARALLEL_JOBS = 8

# Apply settings to build_config instance

def apply_user_config(build_config):
    """Apply user configuration to the BuildConfig object (called by the automation)."""
    if USER_CMAKE_PATH:
        build_config.CMAKE_PATH = USER_CMAKE_PATH
    if USER_NINJA_PATH:
        build_config.NINJA_PATH = USER_NINJA_PATH
    if USER_VCVARSALL_PATH:
        build_config.VCVARSALL_PATH = USER_VCVARSALL_PATH

    # Defaults
    try:
        # The apply_user_config is called with BuildConfig class object. To access
        # enum types (IDE etc.) we import the module where BuildConfig is defined
        # using build_config.__module__ and read IDE from it.
        try:
            module_name = build_config.__module__
            mod = __import__(module_name, fromlist=['IDE'])
            IDE = getattr(mod, 'IDE', None)
        except Exception:
            IDE = None

        if IDE and DEFAULT_IDE.lower() in ("vs", "visual-studio"):
            build_config.current_ide = IDE.VISUAL_STUDIO
    except Exception as e:
        print(f"⚠ Error applying user config: {e}")

    # user can also set other global defaults if desired

# End of file
