# Centralized FetchContent-based dependency management for LevEngine
# This file expects `cmake/ThirdPartyVersions.cmake` to be included by the top-level CMakeLists.txt

include(FetchContent)

# Keep backwards-compatible policy for FetchContent_Populate usage
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

option(LEVENGINE_USE_SUBMODULES "Prefer using vendor/ subdirectories instead of FetchContent" OFF)

if(LEVENGINE_USE_SUBMODULES)
    message(STATUS "LEVEngine: Using vendor/ subdirectories for third-party libs (submodules)")
else()
    message(STATUS "LEVEngine: Using FetchContent to download third-party libs")

    # GLFW
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY ${LEVENGINE_GLFW_GIT_REPOSITORY}
        GIT_TAG ${LEVENGINE_GLFW_GIT_TAG}
    )

    # Prefer local vendor GLEW if present (user-provided). Otherwise fetch release ZIP.
    if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glew")
        message(STATUS "Using local vendor/glew instead of FetchContent")
        if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glew/build/cmake/CMakeLists.txt")
            set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
            add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glew/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/vendor_glew_build")
        elseif(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glew/include/GL/glew.h")
            include_directories(${CMAKE_SOURCE_DIR}/vendor/glew/include)
        else()
            message(WARNING "Found vendor/glew but it does not contain build or include; falling back to fetching release ZIP")
            FetchContent_Declare(
                glew
                URL https://github.com/nigels-com/glew/archive/refs/tags/glew-2.2.0.zip
                DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            )
        endif()
    else()
        # GLEW via release ZIP
        FetchContent_Declare(
            glew
            URL https://github.com/nigels-com/glew/archive/refs/tags/glew-2.2.0.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
    endif()

    # Make GLFW available (preferred)
    FetchContent_MakeAvailable(glfw)

    # If we declared glew via FetchContent, only then populate and consider configuring it
    if(DEFINED glew_SOURCE_DIR)
        # If we've downloaded glew via FetchContent, check headers and optionally configure
        if(NOT TARGET glew_s AND NOT TARGET glew)
            # If glew was declared and populated by FetchContent, _SOURCE_DIR will be defined
            if(NOT "${glew_SOURCE_DIR}" STREQUAL "")
                FetchContent_Populate(glew)
                set(_glew_include_dir "${glew_SOURCE_DIR}/include/GL")
                if(EXISTS "${_glew_include_dir}/glew.h" AND EXISTS "${glew_SOURCE_DIR}/build/cmake/CMakeLists.txt")
                    message(STATUS "GLEW: found generated headers in fetched source; configuring glew build")
                    set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "Allow configuring older projects requiring older CMake" FORCE)
                    file(READ "${glew_SOURCE_DIR}/build/cmake/CMakeLists.txt" _glew_cmake_content)
                    string(REGEX REPLACE "cmake_minimum_required[[:space:]]*\\([^\\n\\r]*\\)" "cmake_minimum_required(VERSION 3.31.6)" _glew_cmake_content "${_glew_cmake_content}")
                    file(WRITE "${glew_SOURCE_DIR}/build/cmake/CMakeLists.txt" "${_glew_cmake_content}")
                    set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
                    add_subdirectory(${glew_SOURCE_DIR}/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
                else()
                    message(WARNING "Fetched GLEW archive does not contain generated headers or build/cmake; skipping building fetched GLEW and relying on vendor or headers-only mode")
                endif()
            endif()
        endif()
    endif()

endif()

# Provide helper alias targets for the rest of the project to link against.
# If upstream created targets, prefer those; otherwise map to legacy names.
if(TARGET glfw)
    add_library(glfw::glfw ALIAS glfw)
endif()
if(TARGET glew_s)
    add_library(glew::glew_s ALIAS glew_s)
endif()
