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
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /ZI")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")

    # Ensure linker generates debug info and allows incremental linking for Debug
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
endif ()

# build only on Windows
if (NOT WIN32)
    message(FATAL_ERROR "This project is configured to build only on Windows (WIN32). Aborting.")
endif ()

# version variables (allow override from cache)
if (NOT DEFINED LEN_VERSION)
    set(LEN_VERSION ${PROJECT_VERSION} CACHE STRING "LEN version")
endif ()
if (NOT DEFINED LEN_VERSION_MAJOR)
    set(LEN_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "LEN version major")
endif ()
if (NOT DEFINED LEN_VERSION_MINOR)
    set(LEN_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "LEN version minor")
endif ()
if (NOT DEFINED LEN_VERSION_PATCH)
    set(LEN_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "LEN version patch")
endif ()

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# If vendor GLFW exists inside Engine add it early so targets are available to Engine and App
if (EXISTS "${CMAKE_SOURCE_DIR}/Engine/vendor/glfw/CMakeLists.txt")
    message(STATUS "TOP: Found vendor GLFW at ${CMAKE_SOURCE_DIR}/Engine/vendor/glfw - adding early")
    add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/vendor/glfw" "${CMAKE_BINARY_DIR}/vendor_glfw_build_top")
endif ()

# Option to create a separate test build of the application with Tracy enabled
option(BUILD_TRACY_TEST "Create separate application target with Tracy profiler enabled (LENAppTracy)" OFF)
message(STATUS "TOP: BUILD_TRACY_TEST=${BUILD_TRACY_TEST}")

# If Tracy profiler is present under external/tracy, add it early so subdirectories can link to it
if (EXISTS "${CMAKE_SOURCE_DIR}/external/tracy/CMakeLists.txt")
    message(STATUS "TOP: Adding external/tracy subdirectory for Tracy profiling (early)")
    add_subdirectory("${CMAKE_SOURCE_DIR}/external/tracy" "${CMAKE_BINARY_DIR}/external_tracy_build")
    set(_TRACY_AVAILABLE ON)
else ()
    message(STATUS "TOP: external/tracy not found; Tracy will be disabled. To enable, add external/tracy to the repository.")
    set(_TRACY_AVAILABLE OFF)
endif ()

# --- cgltf: tiny glTF loader (https://github.com/jkuhlmann/cgltf)
# Prefer local vendor/cgltf, fall back to external/cgltf, else optionally fetch into vendor/cgltf
set(_CGTLF_LOCAL_VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor/cgltf")
set(_CGTLF_EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external/cgltf")

if (EXISTS "${_CGTLF_LOCAL_VENDOR_DIR}")
    message(STATUS "TOP: Found vendor/cgltf at ${_CGTLF_LOCAL_VENDOR_DIR}, using local vendor copy")
    add_library(cgltf STATIC ${_CGTLF_LOCAL_VENDOR_DIR}/cgltf.c)
    target_include_directories(cgltf PUBLIC ${_CGTLF_LOCAL_VENDOR_DIR})
elseif (EXISTS "${_CGTLF_EXTERNAL_DIR}")
    message(STATUS "TOP: Found external/cgltf at ${_CGTLF_EXTERNAL_DIR}, using external copy")
    add_library(cgltf STATIC ${_CGTLF_EXTERNAL_DIR}/cgltf.c)
    target_include_directories(cgltf PUBLIC ${_CGTLF_EXTERNAL_DIR})
else ()
    if (ENGINE_ALLOW_FETCHCONTENT)
        include(FetchContent)
        FetchContent_Declare(
                cgltf
                URL https://github.com/jkuhlmann/cgltf/archive/refs/heads/master.zip
        )
        FetchContent_GetProperties(cgltf)
        if (NOT cgltf_POPULATED)
            message(STATUS "TOP: Fetching cgltf from GitHub into build dir")
            FetchContent_Populate(cgltf)
        endif ()

        # Ensure vendor directory exists and copy fetched source into vendor/cgltf if not already present
        if (NOT EXISTS "${CMAKE_SOURCE_DIR}/vendor")
            file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor")
        endif ()
        if (NOT EXISTS "${_CGTLF_LOCAL_VENDOR_DIR}")
            message(STATUS "TOP: Copying fetched cgltf into ${_CGTLF_LOCAL_VENDOR_DIR}")
            file(MAKE_DIRECTORY "${_CGTLF_LOCAL_VENDOR_DIR}")
            # copy only necessary files to keep vendor clean
            if (EXISTS "${cgltf_SOURCE_DIR}/cgltf.c")
                file(COPY "${cgltf_SOURCE_DIR}/cgltf.c" DESTINATION "${_CGTLF_LOCAL_VENDOR_DIR}")
            endif ()
            if (EXISTS "${cgltf_SOURCE_DIR}/cgltf.h")
                file(COPY "${cgltf_SOURCE_DIR}/cgltf.h" DESTINATION "${_CGTLF_LOCAL_VENDOR_DIR}")
            endif ()
        else ()
            message(STATUS "TOP: vendor/cgltf already exists, skipping copy")
        endif ()

        # add target from vendor copy (so source tree contains the library)
        if (EXISTS "${_CGTLF_LOCAL_VENDOR_DIR}/cgltf.c")
            add_library(cgltf STATIC ${_CGTLF_LOCAL_VENDOR_DIR}/cgltf.c)
            target_include_directories(cgltf PUBLIC ${_CGTLF_LOCAL_VENDOR_DIR})
            message(STATUS "TOP: cgltf target created from ${_CGTLF_LOCAL_VENDOR_DIR}")
        else ()
            message(WARNING "TOP: cgltf was fetched but ${_CGTLF_LOCAL_VENDOR_DIR}/cgltf.c not found; falling back to build-time target from fetched source")
            add_library(cgltf STATIC ${cgltf_SOURCE_DIR}/cgltf.c)
            target_include_directories(cgltf PUBLIC ${cgltf_SOURCE_DIR})
        endif ()
    else ()
        message(STATUS "TOP: cgltf not found in vendor or external, and ENGINE_ALLOW_FETCHCONTENT=OFF — skipping cgltf. To enable fetching set ENGINE_ALLOW_FETCHCONTENT=ON or add vendor/cgltf to the repo.")
    endif ()
endif ()

# Generate EngineConfig.h from template into build dir so the app can include it
if (EXISTS "${CMAKE_SOURCE_DIR}/EngineConfig.h.template")
    configure_file(${CMAKE_SOURCE_DIR}/EngineConfig.h.template ${CMAKE_BINARY_DIR}/EngineConfig.h @ONLY)
    message(STATUS "Generated EngineConfig.h -> ${CMAKE_BINARY_DIR}/EngineConfig.h")
endif ()

# Set startup project for IDEs
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LENApp)

# Optional helper includes
set(_helpers_path "${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake")
if (EXISTS "${_helpers_path}")
    include("${_helpers_path}")
    if (COMMAND system_info)
        system_info()
    endif ()
else ()
    message(STATUS "Optional CMake helpers not found: ${_helpers_path} — continuing without them.")
endif ()

add_subdirectory(Engine)

# Add application subdirectory which creates LENApp from App/Source
add_subdirectory(App)

# Install
install(TARGETS LENApp RUNTIME DESTINATION bin)

# CPack (optional)
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
endif ()
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
