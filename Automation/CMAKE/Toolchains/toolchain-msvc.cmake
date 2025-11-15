# Toolchain for MSVC with Ninja
set(CMAKE_SYSTEM_NAME Windows)

# Use cl.exe detected from environment (vcvarsall)
# If not set, CMake will try to locate MSVC automatically.
# You can override explicitly via -DCMAKE_C_COMPILER / -DCMAKE_CXX_COMPILER

# Ensure MSVC runtime selection through standard flags
if (NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

