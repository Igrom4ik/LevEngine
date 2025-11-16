# MinGW (GCC) toolchain for Windows with Ninja
# Enforces MinGW compilers and prevents MSVC auto-detection

# Target system
set(CMAKE_SYSTEM_NAME Windows)

# Prefer MinGW compilers; allow override via -DCMAKE_* if provided
if (NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif ()
if (NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif ()
# Resource compiler (optional)
find_program(MINGW_RC_EXECUTABLE NAMES windres)
if (MINGW_RC_EXECUTABLE)
    set(CMAKE_RC_COMPILER ${MINGW_RC_EXECUTABLE})
endif ()

# Prevent CMake from searching MSVC toolchain
set(CMAKE_C_COMPILER_ID_RUN TRUE)
set(CMAKE_CXX_COMPILER_ID_RUN TRUE)

# Search modes for MinGW on Windows
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# Reasonable defaults
if (NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    # Not used under MinGW, but silence policies
    set(CMAKE_MSVC_RUNTIME_LIBRARY "")
endif ()

# Prefer static libgcc/libstdc++ in Release to reduce runtime deps (optional)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif ()

# Ensure 64-bit by default when PATH points to mingw64 or ucrt64 (handled by chosen gcc)
# You can force 32-bit by selecting an appropriate toolchain bin (mingw32) or by setting compilers explicitly.

