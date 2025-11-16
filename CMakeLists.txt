# general info
cmake_minimum_required(VERSION 3.31.6)

# cpp options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# project
project(LevEngine VERSION 1.0.0 LANGUAGES C CXX)

# build only on Windows
if (NOT WIN32)
    message(FATAL_ERROR "This project is configured to build only on Windows (WIN32). Aborting.")
endif()

# version variables (allow override from cache)
if (NOT DEFINED LevEngine_VERSION)
    set(LevEngine_VERSION ${PROJECT_VERSION} CACHE STRING "LevEngine version")
endif()
if (NOT DEFINED LevEngine_VERSION_MAJOR)
    set(LevEngine_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "LevEngine version major")
endif()
if (NOT DEFINED LevEngine_VERSION_MINOR)
    set(LevEngine_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "LevEngine version minor")
endif()
if (NOT DEFINED LevEngine_VERSION_PATCH)
    set(LevEngine_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "LevEngine version patch")
endif()

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

add_subdirectory(Engine)

# Add application subdirectory which creates LevEngineApp from App/Source
add_subdirectory(App)

# Generate EngineConfig.h from template into build dir so the app can include it
if (EXISTS "${CMAKE_SOURCE_DIR}/EngineConfig.h.template")
    configure_file(${CMAKE_SOURCE_DIR}/EngineConfig.h.template ${CMAKE_BINARY_DIR}/EngineConfig.h @ONLY)
    message(STATUS "Generated EngineConfig.h -> ${CMAKE_BINARY_DIR}/EngineConfig.h")
endif()

# Set startup project for IDEs
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LevEngineApp)

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
install(TARGETS LevEngineApp RUNTIME DESTINATION bin)

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
    COMMAND ${CMAKE_COMMAND} -E echo "LevEngine version: ${PROJECT_VERSION}"
    COMMENT "Print configured LevEngine version"
)

add_custom_target(package-release-help
    COMMAND ${CMAKE_COMMAND} -E echo "Run: cmake --build <build-dir> --target package-release --config Release"
    COMMENT "Show how to run package-release target"
)
