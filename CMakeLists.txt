cmake_minimum_required(VERSION 3.31.6)
project(LevEngine VERSION 1.0.0 LANGUAGES CXX)

# Ensure older third-party projects that use very old cmake_minimum_required can still be configured
if (NOT DEFINED CMAKE_POLICY_VERSION_MINIMUM)
    set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "Allow configuring older projects requiring older CMake" FORCE)
endif()

# Проверка: если компилятор MSVC и используется Ninja, убедиться, что VS dev environment активирован
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_GENERATOR STREQUAL "Ninja" AND NOT DEFINED ENV{INCLUDE})
        message(FATAL_ERROR "MSVC + Ninja requires Visual Studio developer environment.\nPlease run CMake from a Developer Command Prompt or run the VsDevCmd script first.\nExample (PowerShell):\n  & 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\Community\\Common7\\Tools\\VsDevCmd.bat' -arch=amd64; cmake -S . -B build -G Ninja\nOr use the Visual Studio generator: cmake -G 'Visual Studio 17 2022' -A x64 -S . -B build")
    endif()
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)

# Include centralized third-party version pins and dependency helper
include(cmake/ThirdPartyVersions.cmake)
include(cmake/Dependencies.cmake)

set(PROJECT_SOURCE_FILES
    source/main.cpp
)

include_directories(source)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

# Option to fetch thirdparty libraries via CMake FetchContent (default ON)
option(LEVENGINE_FETCH_THIRDPARTY "Download thirdparty libs via FetchContent" ON)

# Keep backwards-compatible policy for older CMake behavior used in some thirdparty CMakeLists
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

if(LEVENGINE_FETCH_THIRDPARTY AND NOT LEVENGINE_USE_SUBMODULES)
    message(STATUS "LEVEngine: Fetching thirdparty libraries via FetchContent (centralized pins)")

    # Use dependencies declared in cmake/Dependencies.cmake
    # Dependencies.cmake already calls FetchContent_MakeAvailable for glfw and glew when LEVENGINE_USE_SUBMODULES=OFF

    # Fallback handling for older archives that do not create modern targets is still performed below
    if(NOT TARGET glfw)
        if(DEFINED glfw_SOURCE_DIR)
            add_subdirectory(${glfw_SOURCE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
        endif()
    endif()

    if(NOT TARGET glew_s AND NOT TARGET glew)
        if(DEFINED glew_SOURCE_DIR)
            # Only add fetched GLEW's build if the fetched archive contains generated headers
            if(EXISTS "${glew_SOURCE_DIR}/include/GL/glew.h" AND EXISTS "${glew_SOURCE_DIR}/build/cmake/CMakeLists.txt")
                set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
                add_subdirectory(${glew_SOURCE_DIR}/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
            else()
                message(WARNING "Fetched GLEW archive missing generated headers or build/cmake entrypoint; skipping add_subdirectory for fetched GLEW and falling back to vendor or headers-only mode.")
            endif()
        endif()
    endif()

else()
    message(STATUS "LEVEngine: Using local vendor/ directories (not FetchContent or using submodules)")
    # Add local vendor directories if present
    if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/CMakeLists.txt")
        add_subdirectory(vendor/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4")
        include_directories(vendor/glfw-3.4/include)
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glew/build/cmake/CMakeLists.txt")
        set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
        add_subdirectory(vendor/glew/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glew/include")
        include_directories(vendor/glew/include)
    endif()
endif()

# Ensure vendor include dirs are added to the target if they exist
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/include")
    target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/include")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glew/include")
    target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glew/include")
endif()

# Link thirdparty libraries to the executable
# Prefer modern imported targets if available, otherwise legacy names
if(TARGET glfw::glfw)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw::glfw)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
endif()

if(TARGET glew::glew_s)
    target_link_libraries(${PROJECT_NAME} PRIVATE glew::glew_s)
elseif(TARGET glew_s)
    target_link_libraries(${PROJECT_NAME} PRIVATE glew_s)
else()
    # No glew target available; leave it up to the vendor includes if present
endif()

# --- Simple versioning and packaging (minimal, easy to understand) ---
# Allow overriding LevEngine version via -DLEVENGINE_VERSION:STRING=2.3.4
if (NOT DEFINED LEVENGINE_VERSION)
    set(LEVENGINE_VERSION ${PROJECT_VERSION} CACHE STRING "LevEngine version")
endif()
if (NOT DEFINED LEVENGINE_VERSION_MAJOR)
    set(LEVENGINE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "LevEngine version major")
endif()
if (NOT DEFINED LEVENGINE_VERSION_MINOR)
    set(LEVENGINE_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "LevEngine version minor")
endif()
if (NOT DEFINED LEVENGINE_VERSION_PATCH)
    set(LEVENGINE_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "LevEngine version patch")
endif()

# Generate version.txt in build directory for scripts/CI to read (if template exists)
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/version.txt.in")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/version.txt.in ${CMAKE_BINARY_DIR}/version.txt @ONLY)
endif()

# Put build outputs to a single place (simple, predictable)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Useful global properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# Install the main executable so CPack can package it
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Install README so packages include project README at top-level
install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION .)

# Load release-specific CPack defaults if present
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
endif()

# Ensure CPack module is loaded (generates 'package' target)
# Disable wrapping files into a top-level directory inside the ZIP so README.md appears at archive root
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)

# Helper target: build, install into CMAKE_INSTALL_PREFIX and then run package
add_custom_target(package-release
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target install
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target package
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Build Release, install to ${CMAKE_INSTALL_PREFIX} and create release ZIP via CPack")

# Helper target to print current configured version
add_custom_target(print-version
        COMMAND ${CMAKE_COMMAND} -E echo "LevEngine version: ${PROJECT_VERSION}"
        COMMENT "Print configured LevEngine version")

# Shortcut help target
add_custom_target(package-release-help
        COMMAND ${CMAKE_COMMAND} -E echo "Run: cmake --build <build-dir> --target package-release --config Release"
        COMMENT "Show how to run package-release target")
