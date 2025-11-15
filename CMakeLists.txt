# general info
cmake_minimum_required(VERSION 3.31.6)

# cpp options
set(CMAKE_CXX_STANDARD 23)

# common options
project(LevEngine VERSION 1.0.0 LANGUAGES CXX)

# --- Platform restriction: Windows-only build ---
if (NOT WIN32)
    message(FATAL_ERROR "This project is configured to build only on Windows (WIN32). Aborting.")
endif()

# Initialize LevEngine version variables early so they can be overridden via -D
# Allow override from the cache (e.g. -DLevEngine_VERSION:STRING=2.3.4)
if (NOT DEFINED LevEngine_VERSION)
    set(LevEngine_VERSION ${PROJECT_VERSION} CACHE STRING "LevEngine version")
endif ()
if (NOT DEFINED LevEngine_VERSION_MAJOR)
    set(LevEngine_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "LevEngine version major")
endif ()
if (NOT DEFINED LevEngine_VERSION_MINOR)
    set(LevEngine_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "LevEngine version minor")
endif ()
if (NOT DEFINED LevEngine_VERSION_PATCH)
    set(LevEngine_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "LevEngine version patch")
endif ()

# Generate version.txt in build directory for scripts/CI to read (early)
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/version.txt.in")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/version.txt.in ${CMAKE_BINARY_DIR}/version.txt @ONLY)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# --- Vendor third-party libraries (optional) ---
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
set(VENDOR_LIBS "")

# Option: fetch GLFW from git into vendor/ (helps to track versions)
option(VENDOR_GLFW_FROM_GIT "Fetch GLFW from GitHub into vendor/ via FetchContent" ON)
# Default to a known stable GLFW release tag without leading 'v' to avoid invalid-reference errors; users can override with -DVENDOR_GLFW_GIT_TAG
set(VENDOR_GLFW_GIT_TAG "3.3.8" CACHE STRING "GLFW git tag or commit to checkout when VENDOR_GLFW_FROM_GIT is ON")

# If requested, use FetchContent to download GLFW into the vendor folder
if (VENDOR_GLFW_FROM_GIT)
    include(FetchContent)
    # place downloads/sources under vendor/ so they are visible in repo tree
    set(FETCHCONTENT_BASE_DIR "${VENDOR_DIR}" CACHE PATH "FetchContent base dir")
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG ${VENDOR_GLFW_GIT_TAG}
    )
    message(STATUS "VENDOR: fetching GLFW from ${FetchContent_glfw_SOURCE_DIR} (tag=${VENDOR_GLFW_GIT_TAG}) into ${FETCHCONTENT_BASE_DIR}")
    FetchContent_MakeAvailable(glfw)
    # GLFW provides target `glfw` when added this way
    if (TARGET glfw)
        list(APPEND VENDOR_LIBS glfw)
        include_directories("${FetchContent_glfw_SOURCE_DIR}/include")
    endif()
endif()

# GLFW (fallback to local vendor directory if FetchContent was disabled or failed)
if (NOT VENDOR_GLFW_FROM_GIT)
    if (EXISTS "${VENDOR_DIR}/glfw-3.4/CMakeLists.txt")
        add_subdirectory("${VENDOR_DIR}/glfw-3.4" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glfw_build")
        include_directories("${VENDOR_DIR}/glfw-3.4/include")
        list(APPEND VENDOR_LIBS glfw)
    endif()
endif()

# GLEW (expected layout: vendor/glew/include and vendor/glew/build/cmake)
# Support multiple locations: vendor/glew (new), thirdparty/glew (existing in repo)
set(GLEW_SEARCH_CANDIDATES
    "${VENDOR_DIR}/glew"
    "${CMAKE_SOURCE_DIR}/thirdparty/glew"
)
set(_glew_cmake_found FALSE)
foreach(_gpath IN LISTS GLEW_SEARCH_CANDIDATES)
    if (EXISTS "${_gpath}/build/cmake/CMakeLists.txt")
        set(_glew_cmake_dir "${_gpath}/build/cmake")
        set(_glew_include_dir "${_gpath}/include")
        set(_glew_cmake_found TRUE)
        break()
    endif()
endforeach()

if (_glew_cmake_found)
    # keep existing GLEW option name to avoid changing behavior
    set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
    add_subdirectory("${_glew_cmake_dir}" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glew_build")
    include_directories("${_glew_include_dir}")
    if (TARGET glew_s)
        list(APPEND VENDOR_LIBS glew_s)
    elseif (TARGET glew)
        list(APPEND VENDOR_LIBS glew)
    endif()
else()
    # No GLEW cmake scripts found — try to detect prebuilt GLEW in candidate folders (include + lib)
    foreach(_gpath IN LISTS GLEW_SEARCH_CANDIDATES)
        if (EXISTS "${_gpath}/include")
            set(_glew_include_dir "${_gpath}/include")
            # try common MSVC release library path
            set(_glew_lib_x64 "${_gpath}/lib/Release/x64/glew32s.lib")
            set(_glew_lib_x64_dyn "${_gpath}/lib/Release/x64/glew32.lib")
            if (EXISTS "${_glew_lib_x64}")
                add_library(glew_s STATIC IMPORTED)
                set_target_properties(glew_s PROPERTIES IMPORTED_LOCATION "${_glew_lib_x64}")
                include_directories("${_glew_include_dir}")
                list(APPEND VENDOR_LIBS glew_s)
                break()
            elseif (EXISTS "${_glew_lib_x64_dyn}")
                add_library(glew IMPORTED)
                set_target_properties(glew PROPERTIES IMPORTED_LOCATION "${_glew_lib_x64_dyn}")
                include_directories("${_glew_include_dir}")
                list(APPEND VENDOR_LIBS glew)
                break()
            endif()
        endif()
    endforeach()
endif()

# Export the vendor libs list into cache so subprojects (App) can pick it up
set(PROJECT_VENDOR_LIBS "${VENDOR_LIBS}" CACHE STRING "Vendor libraries to link (from vendor/)" FORCE)

# project structure
# Note: engine subdirectory is intentionally NOT added — this build does not depend on the engine library

# Create top-level application target (main.cpp at repo root)
add_executable(LevEngineApp ${CMAKE_SOURCE_DIR}/main.cpp)
# Link vendor libraries if any
if (DEFINED PROJECT_VENDOR_LIBS AND NOT "${PROJECT_VENDOR_LIBS}" STREQUAL "")
    message(STATUS "Linking vendor libraries to LevEngineApp: ${PROJECT_VENDOR_LIBS}")
    target_link_libraries(LevEngineApp PRIVATE ${PROJECT_VENDOR_LIBS})
endif()
# Make sure the executable can see generated headers in the build dir (if any)
target_include_directories(LevEngineApp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LevEngineApp)

# Подключаем дополнительные CMake-хелперы только если файл существует
set(_helpers_path "${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake")
if (EXISTS "${_helpers_path}")
    include("${_helpers_path}")
    if (COMMAND system_info)
        system_info()
    endif ()
else ()
    message(WARNING "Optional CMake helpers not found: ${_helpers_path}. Continuing without it.")
endif ()

# --- CPack (packaging) ---
install(TARGETS LevEngineApp RUNTIME DESTINATION bin)
# Install package documentation: prefer PACKAGE_README.md (renamed to README.md), fallback to README.md
if (EXISTS "${CMAKE_SOURCE_DIR}/PACKAGE_README.md")
    install(FILES "${CMAKE_SOURCE_DIR}/PACKAGE_README.md" DESTINATION ../LevEngine RENAME README.md)
elseif (EXISTS "${CMAKE_SOURCE_DIR}/README.md")
    install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION ../LevEngine)
else ()
    message(WARNING "No PACKAGE_README.md or README.md found — package will not contain README")
endif ()

# Load release-specific CPack defaults (create cmake/CPackReleaseConfig.cmake)
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
endif ()

# Ensure CPack module is loaded (this generates package targets such as 'package')
include(CPack)

# Helper target: build, install into CMAKE_INSTALL_PREFIX and then run packaging target
add_custom_target(package-release
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target install
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target package
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Build Release, install to ${CMAKE_INSTALL_PREFIX} and create release ZIP via CPack"
)

# Helper target to print current configured version
add_custom_target(print-version
        COMMAND ${CMAKE_COMMAND} -E echo "LevEngine version: ${PROJECT_VERSION}"
        COMMENT "Print configured LevEngine version"
)

# Shortcut to run package-release from command line: 'cmake --build . --target package-release'
add_custom_target(package-release-help
        COMMAND ${CMAKE_COMMAND} -E echo "Run: cmake --build <build-dir> --target package-release --config Release"
        COMMENT "Show how to run package-release target"
)
