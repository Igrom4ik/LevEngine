"""
HuyEngine - Build Configuration
Configuration for various IDEs and build systems
"""

import json
import os
from enum import Enum
from pathlib import Path


class IDE(Enum):
    """Supported IDEs"""
    VISUAL_STUDIO = "Visual Studio"
    CLION = "CLion"
    VSCODE = "VSCode"
    MANUAL = "Manual (Command Line)"


class BuildSystem(Enum):
    """Supported build systems"""
    NINJA = "Ninja"
    MSBUILD = "MSBuild (Visual Studio)"
    MAKE = "Unix Makefiles"


class Configuration(Enum):
    """Build configurations"""
    DEBUG = "Debug"
    RELEASE = "Release"
    RELWITHDEBINFO = "RelWithDebInfo"
    MINSIZEREL = "MinSizeRel"


class Platform(Enum):
    """Platforms"""
    X64 = "x64"
    WIN32 = "Win32"
    ARM64 = "ARM64"


class Toolchain(Enum):
    """Compiler toolchains"""
    MSVC = "MSVC (cl)"
    CLANG_CL = "Clang-CL (LLVM with MSVC)"
    MINGW_GCC = "MinGW (GCC)"


class BuildConfig:
    """Main build configuration"""

    # Project root directory
    PROJECT_ROOT = Path(__file__).parent.parent

    # Файл состояния (персистентная конфигурация между запусками)
    STATE_FILE = PROJECT_ROOT / "Automation" / ".build_state.json"

    # Project name and C++ standard (can be overridden by menu)
    PROJECT_NAME = "HuyEngine"
    CXX_STANDARD = 20

    # CMake minimum required version (can be set in menu)
    CMAKE_MINIMUM_VERSION = "3.31.6"

    # Executable name (legacy default); prefer get_executable_name() in code
    EXECUTABLE_NAME = "HuyEngine.exe"

    # Source directories for clang-format
    SOURCE_DIRS = ["Engine", "main.cpp"]

    # Tool paths (will be automatically detected)
    CMAKE_PATH = None
    NINJA_PATH = None
    CXX_COMPILER = None
    C_COMPILER = None
    VCVARSALL_PATH = None

    # Additional compiler tool paths
    CLANG_CL_PATH = None
    CLANGXX_PATH = None
    GCC_PATH = None
    GPP_PATH = None

    # Current settings
    current_ide = IDE.MANUAL
    current_build_system = BuildSystem.NINJA
    current_configuration = Configuration.DEBUG
    current_platform = Platform.X64
    current_toolchain = Toolchain.MSVC

    # Build options
    verbose = False
    clean_first = True

    @staticmethod
    def get_build_folder():
        """Returns the build folder depending on IDE and build system"""
        if BuildConfig.current_ide == IDE.CLION:
            if BuildConfig.current_configuration == Configuration.DEBUG:
                return "cmake-build-debug"
            elif BuildConfig.current_configuration == Configuration.RELEASE:
                return "cmake-build-release"
            else:
                return f"cmake-build-{BuildConfig.current_configuration.value.lower()}"
        elif BuildConfig.current_ide == IDE.VISUAL_STUDIO:
            return "vs-build"
        else:
            # Separate build folders per toolchain to avoid cache collisions
            tc = BuildConfig.current_toolchain
            if tc == Toolchain.MSVC:
                return "build-msvc"
            elif tc == Toolchain.CLANG_CL:
                return "build-clang-cl"
            elif tc == Toolchain.MINGW_GCC:
                return "build-mingw"
            return "build"

    @staticmethod
    def get_cmake_generator():
        """Returns CMake generator depending on the build system"""
        if BuildConfig.current_build_system == BuildSystem.NINJA:
            return "Ninja"
        elif BuildConfig.current_build_system == BuildSystem.MSBUILD:
            # Detect Visual Studio version
            vs_versions = [
                ("Visual Studio 17 2022", r"C:\\Program Files\\Microsoft Visual Studio\\2022"),
                ("Visual Studio 16 2019", r"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019"),
            ]
            for gen, path in vs_versions:
                if os.path.exists(path):
                    return gen
            return "Visual Studio 17 2022"  # Default
        elif BuildConfig.current_build_system == BuildSystem.MAKE:
            return "Unix Makefiles"
        return "Ninja"

    @staticmethod
    def get_toolchain_file():
        """Return path to CMake toolchain file for the selected toolchain (for CMake + Ninja)."""
        tc_dir = BuildConfig.PROJECT_ROOT / "Automation" / "CMAKE" / "Toolchains"
        if BuildConfig.current_toolchain == Toolchain.MSVC:
            return str(tc_dir / "toolchain-msvc.cmake")
        elif BuildConfig.current_toolchain == Toolchain.CLANG_CL:
            return str(tc_dir / "toolchain-clang-cl.cmake")
        elif BuildConfig.current_toolchain == Toolchain.MINGW_GCC:
            return str(tc_dir / "toolchain-mingw.cmake")
        return None

    @staticmethod
    def get_project_name():
        """Return project name to use in generated/configured files"""
        return BuildConfig.PROJECT_NAME

    @staticmethod
    def get_executable_name():
        """Return platform-appropriate executable name based on project name"""
        name = BuildConfig.get_project_name()
        # Windows: add .exe suffix
        return f"{name}.exe"

    @staticmethod
    def get_cxx_standard():
        """Return selected C++ standard (int)"""
        return BuildConfig.CXX_STANDARD

    @staticmethod
    def get_cmake_minimum_version():
        """Return selected minimal CMake version as string"""
        return BuildConfig.CMAKE_MINIMUM_VERSION

    @staticmethod
    def load_state():
        """Load persisted state from STATE_FILE if present"""
        try:
            if not BuildConfig.STATE_FILE.exists():
                return False
            with open(BuildConfig.STATE_FILE, "r", encoding="utf-8") as f:
                data = json.load(f)

            # Scalars
            if isinstance(data.get("PROJECT_NAME"), str):
                BuildConfig.PROJECT_NAME = data["PROJECT_NAME"]
            if isinstance(data.get("CXX_STANDARD"), int):
                BuildConfig.CXX_STANDARD = data["CXX_STANDARD"]
            if isinstance(data.get("CMAKE_MINIMUM_VERSION"), str):
                BuildConfig.CMAKE_MINIMUM_VERSION = data["CMAKE_MINIMUM_VERSION"]

            # Enums by name
            def apply_enum(key, enum_cls, attr_name):
                val = data.get(key)
                if isinstance(val, str):
                    try:
                        setattr(BuildConfig, attr_name, enum_cls[val])
                    except KeyError:
                        # Try match by value string too
                        for member in enum_cls:
                            if member.value == val:
                                setattr(BuildConfig, attr_name, member)
                                break

            apply_enum("current_ide", IDE, "current_ide")
            apply_enum("current_build_system", BuildSystem, "current_build_system")
            apply_enum("current_configuration", Configuration, "current_configuration")
            apply_enum("current_platform", Platform, "current_platform")
            apply_enum("current_toolchain", Toolchain, "current_toolchain")

            return True
        except Exception:
            return False

    @staticmethod
    def save_state():
        """Persist current state to STATE_FILE"""
        try:
            payload = {
                "PROJECT_NAME": BuildConfig.PROJECT_NAME,
                "CXX_STANDARD": BuildConfig.CXX_STANDARD,
                "CMAKE_MINIMUM_VERSION": BuildConfig.CMAKE_MINIMUM_VERSION,
                "current_ide": BuildConfig.current_ide.name,
                "current_build_system": BuildConfig.current_build_system.name,
                "current_configuration": BuildConfig.current_configuration.name,
                "current_platform": BuildConfig.current_platform.name,
                "current_toolchain": BuildConfig.current_toolchain.name,
            }
            BuildConfig.STATE_FILE.parent.mkdir(parents=True, exist_ok=True)
            with open(BuildConfig.STATE_FILE, "w", encoding="utf-8") as f:
                json.dump(payload, f, ensure_ascii=False, indent=2)
            return True
        except Exception:
            return False


class ToolPaths:
    """Development tool paths"""

    # Possible CMake paths
    CMAKE_PATHS = [
        r"C:\\Program Files\\CMake\\bin\\cmake.exe",
        r"C:\\Program Files (x86)\\CMake\\bin\\cmake.exe",
        r"cmake",  # From PATH
    ]

    # Possible Ninja paths
    NINJA_PATHS = [
        r"C:\\ProgramData\\chocolatey\\bin\\ninja.exe",
        r"C:\\Program Files\\Ninja\\ninja.exe",
        r"ninja",  # From PATH
    ]

    # Possible Visual Studio paths
    VISUAL_STUDIO_PATHS = [
        # VS 2022
        (r"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community", "2022"),
        (r"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional", "2022"),
        (r"C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise", "2022"),
        # VS 2019
        (r"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community", "2019"),
        (r"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional", "2019"),
        (r"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Enterprise", "2019"),
    ]

    # Possible LLVM/Clang roots
    LLVM_PATHS = [
        r"C:\\Program Files\\LLVM\\bin",
        r"C:\\Program Files (x86)\\LLVM\\bin",
    ]

    # Possible MinGW roots (bin folders)
    MINGW_PATHS = [
        r"C:\\msys64\\mingw64\\bin",
        r"C:\\mingw64\\bin",
        r"C:\\msys64\\ucrt64\\bin",
        r"C:\\msys64\\mingw32\\bin",
    ]

    @staticmethod
    def find_cmake():
        """Find CMake in the system"""
        import shutil
        for path in ToolPaths.CMAKE_PATHS:
            if path == "cmake":
                found = shutil.which("cmake")
                if found:
                    return found
            elif os.path.exists(path):
                return path
        return None

    @staticmethod
    def find_ninja():
        """Find Ninja in the system"""
        import shutil
        for path in ToolPaths.NINJA_PATHS:
            if path == "ninja":
                found = shutil.which("ninja")
                if found:
                    return found
            elif os.path.exists(path):
                return path
        return None

    @staticmethod
    def find_visual_studio():
        """Find Visual Studio in the system"""
        for vs_path, version in ToolPaths.VISUAL_STUDIO_PATHS:
            if os.path.exists(vs_path):
                # Find compilers
                vc_tools = Path(vs_path) / "VC" / "Tools" / "MSVC"
                if vc_tools.exists():
                    # Get the latest compiler version
                    versions = sorted([d for d in vc_tools.iterdir() if d.is_dir()], reverse=True)
                    if versions:
                        msvc_version = versions[0]
                        compiler_path = msvc_version / "bin" / "Hostx64" / "x64" / "cl.exe"
                        vcvarsall = Path(vs_path) / "VC" / "Auxiliary" / "Build" / "vcvarsall.bat"

                        if compiler_path.exists() and vcvarsall.exists():
                            return {
                                "version": version,
                                "path": vs_path,
                                "msvc_version": msvc_version.name,
                                "compiler": str(compiler_path),
                                "vcvarsall": str(vcvarsall)
                            }
        return None

    @staticmethod
    def find_llvm():
        """Find LLVM/Clang (clang-cl and clang++)"""
        import shutil
        # Try PATH first
        clang_cl = shutil.which("clang-cl")
        clang_pp = shutil.which("clang++")
        if clang_cl or clang_pp:
            return {
                "clang_cl": clang_cl,
                "clangxx": clang_pp,
            }
        # Try common install dirs
        for base in ToolPaths.LLVM_PATHS:
            if os.path.exists(base):
                ccl = Path(base) / "clang-cl.exe"
                cpp = Path(base) / "clang++.exe"
                data = {
                    "clang_cl": str(ccl) if ccl.exists() else None,
                    "clangxx": str(cpp) if cpp.exists() else None,
                }
                if data["clang_cl"] or data["clangxx"]:
                    return data
        return {"clang_cl": None, "clangxx": None}

    @staticmethod
    def find_mingw():
        """Find MinGW GCC/G++"""
        import shutil
        gcc = shutil.which("gcc")
        gpp = shutil.which("g++")
        if gcc and gpp:
            return {"gcc": gcc, "gpp": gpp}
        for base in ToolPaths.MINGW_PATHS:
            if os.path.exists(base):
                gcc_p = Path(base) / "gcc.exe"
                gpp_p = Path(base) / "g++.exe"
                if gcc_p.exists() and gpp_p.exists():
                    return {"gcc": str(gcc_p), "gpp": str(gpp_p)}
        return {"gcc": None, "gpp": None}

    @staticmethod
    def initialize_tools():
        """Initialize tool paths"""
        # Find CMake
        BuildConfig.CMAKE_PATH = ToolPaths.find_cmake()
        if not BuildConfig.CMAKE_PATH:
            print("⚠ CMake not found in the system!")

        # Find Ninja
        BuildConfig.NINJA_PATH = ToolPaths.find_ninja()
        if not BuildConfig.NINJA_PATH:
            print("⚠ Ninja not found in the system!")

        # Find Visual Studio
        vs_info = ToolPaths.find_visual_studio()
        if vs_info:
            BuildConfig.CXX_COMPILER = vs_info["compiler"]
            BuildConfig.C_COMPILER = vs_info["compiler"]
            BuildConfig.VCVARSALL_PATH = vs_info["vcvarsall"]
        else:
            print("⚠ Visual Studio not found in the system!")

        # Find LLVM/Clang
        llvm = ToolPaths.find_llvm()
        BuildConfig.CLANG_CL_PATH = llvm.get("clang_cl")
        BuildConfig.CLANGXX_PATH = llvm.get("clangxx")

        # Find MinGW (GCC)
        mingw = ToolPaths.find_mingw()
        BuildConfig.GCC_PATH = mingw.get("gcc")
        BuildConfig.GPP_PATH = mingw.get("gpp")

        # Load user configuration
        ToolPaths.load_user_config()

    @staticmethod
    def load_user_config():
        """Load user configuration if it exists"""
        try:
            import importlib.util
            user_config_path = Path(__file__).parent / "user_config.py"

            if user_config_path.exists():
                spec = importlib.util.spec_from_file_location("user_config", user_config_path)
                user_config = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(user_config)

                # Apply user settings
                if hasattr(user_config, 'apply_user_config'):
                    user_config.apply_user_config(BuildConfig)
                    print("✓ User configuration loaded")
        except Exception as e:
            print(f"⚠ Error loading user configuration: {e}")
