# Toolchain for Clang-CL (LLVM with MSVC environment)
# Use with Ninja generator. Ensure MSVC environment is set (vcvarsall) before configuring.

set(CMAKE_SYSTEM_NAME Windows)

# Find clang-cl compiler if not explicitly set
if (NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    # Common installation paths for clang-cl
    find_program(CLANG_CL_EXECUTABLE
            NAMES clang-cl clang-cl.exe
            PATHS
            # Standalone LLVM installation
            "$ENV{ProgramFiles}/LLVM/bin"
            "$ENV{ProgramFiles\(x86\)}/LLVM/bin"
            "C:/Program Files/LLVM/bin"
            "C:/Program Files (x86)/LLVM/bin"

            # Visual Studio 2022 installations
            "$ENV{ProgramFiles}/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin"
            "$ENV{ProgramFiles}/Microsoft Visual Studio/2022/Professional/VC/Tools/Llvm/x64/bin"
            "$ENV{ProgramFiles}/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin"
            "$ENV{ProgramFiles}/Microsoft Visual Studio/2022/BuildTools/VC/Tools/Llvm/x64/bin"

            # Visual Studio 2019 installations (fallback)
            "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/x64/bin"
            "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/2019/Professional/VC/Tools/Llvm/x64/bin"
            "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/2019/Enterprise/VC/Tools/Llvm/x64/bin"
            DOC "Path to clang-cl compiler"
            NO_DEFAULT_PATH
    )

    # Also try system PATH
    if (NOT CLANG_CL_EXECUTABLE)
        find_program(CLANG_CL_EXECUTABLE NAMES clang-cl clang-cl.exe)
    endif ()

    # Provide helpful error message if not found
    if (NOT CLANG_CL_EXECUTABLE)
        message(FATAL_ERROR
                "clang-cl compiler not found!\n"
                "Please install one of the following:\n"
                "  1. LLVM for Windows from https://github.com/llvm/llvm-project/releases\n"
                "  2. Visual Studio with 'C++ Clang tools for Windows' component\n"
                "  3. Add clang-cl to your PATH\n"
                "Or set CMAKE_C_COMPILER and CMAKE_CXX_COMPILER manually."
        )
    endif ()

    message(STATUS "Found clang-cl: ${CLANG_CL_EXECUTABLE}")

    set(CMAKE_C_COMPILER ${CLANG_CL_EXECUTABLE} CACHE FILEPATH "C compiler")
    set(CMAKE_CXX_COMPILER ${CLANG_CL_EXECUTABLE} CACHE FILEPATH "CXX compiler")
endif ()

# Tell CMake not to second-guess the compiler id
set(CMAKE_C_COMPILER_ID_RUN TRUE)
set(CMAKE_CXX_COMPILER_ID_RUN TRUE)

# Runtime library selection via MSVC-style property
if (NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

