# LevEngine

[![Документация: GitHub Copilot](https://img.shields.io/badge/Документация-GitHub%20Copilot-blue?logo=github)](https://github.com/features/copilot)
[![Windows Build](https://github.com/YOUR_USERNAME/LevEngine/actions/workflows/windows-build.yml/badge.svg)](https://github.com/YOUR_USERNAME/LevEngine/actions/workflows/windows-build.yml)
[![Clang-format check](https://github.com/YOUR_USERNAME/LevEngine/actions/workflows/clang-format-checker.yml/badge.svg)](https://github.com/YOUR_USERNAME/LevEngine/actions/workflows/clang-format-checker.yml)

Custom Game Engine built with C++ and CMake.

> 📝 **Documentation and documentation design are powered by GitHub Copilot**

**Note:** This is an educational project currently in WIP (work-in-progress). The project is released under a permissive
license — everyone is welcome to study, modify, and fork the repository.

## 🚀 Quick Start

### Build System v2.0 (Recommended)

```bash
# Interactive menu (easiest way)
Automation\build.bat

# Or quick commands
Automation\build.bat rebuild    # Full rebuild
Automation\build.bat build      # Build
Automation\build.bat run        # Run
```

**📖 Full documentation:** [docs/README.md](docs/README.md) | [Automation/README.md](Automation/README.md)

### Features

- 🎮 **Interactive menu** - no need to remember commands
- 🛠️ **Multiple IDEs** - Visual Studio, CLion, VSCode support
- ⚡ **Fast builds** - Ninja, MSBuild
- 📁 **Smart folders** - separate folders per configuration
- 🔍 **Auto-detection** - finds CMake, Ninja, Visual Studio automatically

## Build System

### Prerequisites

- CMake 3.31.6+
- Python 3.6+
- Visual Studio 2019/2022 (MSVC)
- Ninja (optional, recommended for speed)

**Quick install (Windows):**

```bash
choco install python cmake ninja
```

## 🔄 CI/CD

Project uses GitHub Actions for continuous integration:

- ✅ **Windows Build** - MSVC compilation (Debug/Release)
- ✅ **Clang-format Check** - Code style validation

All workflows run automatically on push/PR to `master` branch. See [.github/workflows/](.github/workflows/) for details.

### Quick Start

```bash
# Interactive menu
Automation\build.bat

# Command line
Automation\build.bat rebuild              # Full rebuild
Automation\build.bat build                # Build only
Automation\build.bat clean-all            # Clean all
Automation\build.bat run                  # Run app

# With options
Automation\build.bat -i clion -c debug generate
Automation\build.bat -b ninja -c release build
```

### Build Commands (New System v2.0)

| Command                          | Description                   |
|----------------------------------|-------------------------------|
| `Automation\build.bat`           | Interactive menu (easiest)    |
| `Automation\build.bat generate`  | Generate CMake project        |
| `Automation\build.bat build`     | Build project                 |
| `Automation\build.bat rebuild`   | Full rebuild                  |
| `Automation\build.bat clean`     | Clean current folder          |
| `Automation\build.bat clean-all` | Clean all build folders       |
| `Automation\build.bat run`       | Run executable                |
| `Automation\build.bat format`    | Format code with clang-format |

### Examples

```bash
# Interactive menu (recommended)
Automation\build.bat

# Quick commands
Automation\build.bat rebuild
Automation\build.bat run

# With options
Automation\build.bat -i clion -c debug generate
Automation\build.bat -b ninja -c release build

# Release build
Automation\build.bat rebuild --configuration Release
Automation\build.bat run --configuration Release

# Clean build directory
Automation\build.bat clean

# Format code
Automation\build.bat format
```

## Project Structure

```
LevEngine/
├── CMakeLists.txt              # Root CMake configuration (v3.31.6, C++23)
├── CMakePresets.json           # CMake presets for direct CMake usage
├── README.md                   # This file
├── App/                        # Application module
│   ├── CMakeLists.txt          # App CMake configuration
│   ├── EngineConfig.h.template # Version template
│   └── Source/
│       ├── main.cpp            # Entry point
│       ├── pch.cpp/pch.hpp     # Precompiled headers
│       └── Version.h           # Version info
├── Automation/                 # Build automation system v2.0
│   ├── build.bat               # Main build launcher
│   ├── build_menu.bat          # Interactive menu launcher
│   ├── automation_new.py       # Python automation script
│   ├── build_config.py         # Build system configuration
│   ├── build_actions.py        # Build actions
│   ├── build_menu.py           # Interactive menu
│   ├── user_config.template.py # User config template
│   ├── README.md               # Automation documentation
│   └── CMAKE/
│       ├── CmakeHelpers.cmake  # CMake helper functions
│       └── Toolchains/         # Compiler toolchains
├── docs/                       # Documentation (RU)
│   ├── README.md               # Documentation index
│   ├── BUILD_QUICKSTART.md     # Quick start guide
│   ├── BUILD_INSTRUCTIONS_RU.md # Full instruction
│   └── [other docs]
├── scripts/                    # Additional scripts
└── cmake-build-*/              # Build directories (auto-generated)
```

## Configuration

Build settings can be customized:

1. **User Configuration** (Recommended):
   ```bash
   # Copy template
   copy Automation\user_config.template.py Automation\user_config.py
   
   # Edit user_config.py with your preferences
   ```

2. **Project Configuration**:
    - `CMakeLists.txt` - CMake settings (C++23, version 1.0.0)
    - `CMakePresets.json` - CMake presets (otladka, reliz)
    - `Automation/build_config.py` - Build system configuration

Example `user_config.py`:

```python
DEFAULT_IDE = "vs"
DEFAULT_BUILD_SYSTEM = "msbuild"
DEFAULT_CONFIGURATION = "release"

# Custom paths (optional)
USER_CMAKE_PATH = r"D:\Tools\CMake\bin\cmake.exe"
USER_NINJA_PATH = r"D:\Tools\Ninja\ninja.exe"

# Additional CMake flags
CUSTOM_CMAKE_FLAGS = ["-DENABLE_TESTING=ON"]
```

## Development

### Project Structure

- **LevEngine** - Game project simplified to root application
    - Built as `LevEngineApp.exe`
    - No internal `LevEngineLib` (engine library removed)

### Adding New Source Files

**Application:**

1. Add `.cpp` files to `App/Source/`
2. Add headers to `App/Source/`
3. Update `App/CMakeLists.txt` if needed
4. Rebuild: `Automation\build.bat rebuild`

### CMake Features

- **C++23 Standard** (set in root CMakeLists.txt)
- **Versioning** - Automatic version configuration via templates
- **Precompiled Headers** - PCH support for faster compilation (App)
- **IDE Folders** - Source grouping for better organization
- **Output Directories** - All binaries in `${CMAKE_BINARY_DIR}/bin`

### IDE Support

- **CLion**: Open project root, uses `cmake-build-*` folders and CMakePresets.json
- **Visual Studio**: Generate via menu or `Automation\build.bat -i vs -b msbuild generate`
- **VSCode**: See [docs/VSCODE_SETUP.md](docs/VSCODE_SETUP.md)

## 🤖 Документация

Дизайн документации и вся документация проекта созданы благодаря **GitHub Copilot**.

### Документы:

- [docs/BUILD_QUICKSTART.md](docs/BUILD_QUICKSTART.md) - быстрый старт сборки
- [docs/BUILD_INSTRUCTIONS_RU.md](docs/BUILD_INSTRUCTIONS_RU.md) - полная инструкция по сборке
- [docs/CLION_HOTKEYS.md](docs/CLION_HOTKEYS.md) - запуск Build Menu по хоткею в CLion
- [Automation/BUILD_SYSTEM_README.md](Automation/BUILD_SYSTEM_README.md) - документация системы автоматизации
- [Automation/BUILD_EXAMPLES.md](Automation/BUILD_EXAMPLES.md) - примеры использования
- [Automation/BUILD_FAQ.md](Automation/BUILD_FAQ.md) - часто задаваемые вопросы
- [Docs/CameraComponent.md](Docs/CameraComponent.md) - документация по `CameraComponent`: конвенции матриц, пример
  реализации `GetViewMatrix()` и рекомендации по управлению камерой (yaw/pitch).

---

## License

This project is an educational work-in-progress (WIP). It is released under a permissive license (MIT) — feel free to
study, modify and fork the repository.

---

<center>
  <em>Документация и дизайн созданы с помощью <strong>GitHub Copilot</strong> 🤖</em>
</center>

<!-- ci: touch 2025-11-14 -->
