# general info
cmake_minimum_required(VERSION 3.31.6)

# cpp options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# project
project(LEN VERSION 1.0.0 LANGUAGES C CXX)

# Ensure MSVC Debug builds use Edit-and-Continue (/ZI) and do not use /GL or /Zi
if (MSVC)
    # Remove Whole Program Optimization from Debug flags if present
    string(REPLACE "/GL" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/GL" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

    # Remove plain /Zi if present so it does not override /ZI
    string(REPLACE "/Zi" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

    # Use Program Database for Edit & Continue
    set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} /ZI")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")

    # Ensure linker generates debug info and allows incremental linking for Debug
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
endif()

# build only on Windows
if (NOT WIN32)
    message(FATAL_ERROR "This project is configured to build only on Windows (WIN32). Aborting.")
endif()

# version variables (allow override from cache)
if (NOT DEFINED LEN_VERSION)
    set(LEN_VERSION ${PROJECT_VERSION} CACHE STRING "LEN version")
endif()
if (NOT DEFINED LEN_VERSION_MAJOR)
    set(LEN_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "LEN version major")
endif()
if (NOT DEFINED LEN_VERSION_MINOR)
    set(LEN_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "LEN version minor")
endif()
if (NOT DEFINED LEN_VERSION_PATCH)
    set(LEN_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "LEN version patch")
endif()

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# If vendor GLFW exists inside Engine add it early so targets are available to Engine and App
if (EXISTS "${CMAKE_SOURCE_DIR}/Engine/vendor/glfw/CMakeLists.txt")
    message(STATUS "TOP: Found vendor GLFW at ${CMAKE_SOURCE_DIR}/Engine/vendor/glfw - adding early")
    add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/vendor/glfw" "${CMAKE_BINARY_DIR}/vendor_glfw_build_top")
endif()

add_subdirectory(Engine)

# Add application subdirectory which creates LENApp from App/Source
add_subdirectory(App)

# Generate EngineConfig.h from template into build dir so the app can include it
if (EXISTS "${CMAKE_SOURCE_DIR}/EngineConfig.h.template")
    configure_file(${CMAKE_SOURCE_DIR}/EngineConfig.h.template ${CMAKE_BINARY_DIR}/EngineConfig.h @ONLY)
    message(STATUS "Generated EngineConfig.h -> ${CMAKE_BINARY_DIR}/EngineConfig.h")
endif()

# Set startup project for IDEs
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LENApp)

# Optional helper includes
set(_helpers_path "${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake")
if (EXISTS "${_helpers_path}")
    include("${_helpers_path}")
    if (COMMAND system_info)
        system_info()
    endif()
else()
    message(STATUS "Optional CMake helpers not found: ${_helpers_path} — continuing without them.")
endif()

# Install
install(TARGETS LENApp RUNTIME DESTINATION bin)

# CPack (optional)
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
endif()
include(CPack)

# Helper targets
add_custom_target(package-release
    COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target install
    COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target package
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Build Release, install to ${CMAKE_INSTALL_PREFIX} and create release ZIP via CPack"
)

add_custom_target(print-version
    COMMAND ${CMAKE_COMMAND} -E echo "LEN version: ${PROJECT_VERSION}"
    COMMENT "Print configured LEN version"
)

add_custom_target(package-release-help
    COMMAND ${CMAKE_COMMAND} -E echo "Run: cmake --build <build-dir> --target package-release --config Release"
    COMMENT "Show how to run package-release target"
)

# Optional: default to not allowing network fetches in IDEs to avoid blocking Visual Studio configuration
option(ENGINE_ALLOW_FETCHCONTENT "Allow CMake to fetch vendor dependencies from network (OFF recommended for IDEs)" OFF)
message(STATUS "TOP: ENGINE_ALLOW_FETCHCONTENT=${ENGINE_ALLOW_FETCHCONTENT}")
