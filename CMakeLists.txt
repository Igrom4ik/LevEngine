# general info
cmake_minimum_required(VERSION 3.31.6)

# cpp options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# project
project(LevEngine VERSION 1.0.0 LANGUAGES CXX)

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

# --- Vendor libraries ---
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

# Ensure IDEs see vendor includes (helps CLion indexer find headers like <GL/glew.h>)
if (EXISTS "${VENDOR_DIR}/glew/include")
    include_directories("${VENDOR_DIR}/glew/include")
    message(STATUS "VENDOR: added ${VENDOR_DIR}/glew/include to global include paths for IDE/indexer")
endif()

# GLFW via Git clone (safer) or FetchContent
option(VENDOR_GLFW_USE_CLONE "Clone GLFW into ${VENDOR_DIR}/glfw via git (safer)" ON)
option(VENDOR_GLFW_FROM_GIT "Use FetchContent to get GLFW from GitHub" OFF)
set(VENDOR_GLFW_GIT_TAG "3.3.8" CACHE STRING "GLFW git tag or commit to checkout when fetching GLFW")

if (VENDOR_GLFW_USE_CLONE)
    set(_glfw_dir "${VENDOR_DIR}/glfw")
    if (NOT EXISTS "${_glfw_dir}")
        file(MAKE_DIRECTORY "${VENDOR_DIR}")
        message(STATUS "VENDOR: obtaining GLFW into ${_glfw_dir} (tag=${VENDOR_GLFW_GIT_TAG})")
        # 1) Try shallow clone (no tag) then checkout tag if available
        execute_process(COMMAND git clone --depth 1 https://github.com/glfw/glfw.git "${_glfw_dir}"
                        RESULT_VARIABLE _git_clone_res
                        OUTPUT_QUIET ERROR_QUIET)
        if (_git_clone_res EQUAL 0)
            # try to checkout desired tag inside the cloned repo
            execute_process(COMMAND git -C "${_glfw_dir}" fetch --depth 1 origin "refs/tags/${VENDOR_GLFW_GIT_TAG}" RESULT_VARIABLE _git_fetch_res OUTPUT_QUIET ERROR_QUIET)
            if (_git_fetch_res EQUAL 0)
                execute_process(COMMAND git -C "${_glfw_dir}" checkout "tags/${VENDOR_GLFW_GIT_TAG}" RESULT_VARIABLE _git_checkout_res OUTPUT_QUIET ERROR_QUIET)
            else()
                # try with 'v' prefix
                execute_process(COMMAND git -C "${_glfw_dir}" fetch --depth 1 origin "refs/tags/v${VENDOR_GLFW_GIT_TAG}" RESULT_VARIABLE _git_fetch_res2 OUTPUT_QUIET ERROR_QUIET)
                if (_git_fetch_res2 EQUAL 0)
                    execute_process(COMMAND git -C "${_glfw_dir}" checkout "tags/v${VENDOR_GLFW_GIT_TAG}" RESULT_VARIABLE _git_checkout_res OUTPUT_QUIET ERROR_QUIET)
                else()
                    set(_git_checkout_res 0) # nothing to do, keep default branch
                endif()
            endif()
            if (DEFINED _git_checkout_res AND _git_checkout_res EQUAL 0)
                message(STATUS "VENDOR: GLFW cloned into ${_glfw_dir} and checked out tag if available")
            else()
                message(STATUS "VENDOR: GLFW cloned into ${_glfw_dir} (tag checkout not applied)")
            endif()
        else()
            # 2) Try cloning directly by tag name
            execute_process(COMMAND git clone --branch ${VENDOR_GLFW_GIT_TAG} --depth 1 https://github.com/glfw/glfw.git "${_glfw_dir}_tmp"
                            RESULT_VARIABLE _git_tag_clone_res
                            OUTPUT_QUIET ERROR_QUIET)
            if (_git_tag_clone_res EQUAL 0)
                file(RENAME "${_glfw_dir}_tmp" "${_glfw_dir}")
                set(_git_clone_res 0)
                message(STATUS "VENDOR: GLFW cloned by tag ${VENDOR_GLFW_GIT_TAG}")
            else()
                # 3) Try tag with 'v' prefix
                execute_process(COMMAND git clone --branch v${VENDOR_GLFW_GIT_TAG} --depth 1 https://github.com/glfw/glfw.git "${_glfw_dir}_tmp"
                                RESULT_VARIABLE _git_tag_clone_res2
                                OUTPUT_QUIET ERROR_QUIET)
                if (_git_tag_clone_res2 EQUAL 0)
                    file(RENAME "${_glfw_dir}_tmp" "${_glfw_dir}")
                    set(_git_clone_res 0)
                    message(STATUS "VENDOR: GLFW cloned by tag v${VENDOR_GLFW_GIT_TAG}")
                else()
                    # 4) Fallback: try to download zip archive of the tag (no git required)
                    set(_zip_url "https://github.com/glfw/glfw/archive/refs/tags/${VENDOR_GLFW_GIT_TAG}.zip")
                    set(_zip_local "${CMAKE_BINARY_DIR}/glfw-${VENDOR_GLFW_GIT_TAG}.zip")
                    message(STATUS "VENDOR: git clone failed, attempting to download ${_zip_url}")
                    file(DOWNLOAD ${_zip_url} ${_zip_local} SHOW_PROGRESS STATUS _dl_status LOG ERROR)
                    list(GET _dl_status 0 _dl_code)
                    if (_dl_code EQUAL 0)
                        file(ARCHIVE_EXTRACT INPUT ${_zip_local} DESTINATION "${VENDOR_DIR}")
                        # extracted folder name is glfw-<tag>
                        file(GLOB _ex_dirs RELATIVE "${VENDOR_DIR}" "${VENDOR_DIR}/glfw-*")
                        list(GET _ex_dirs 0 _first_ex)
                        if (EXISTS "${VENDOR_DIR}/${_first_ex}")
                            file(RENAME "${VENDOR_DIR}/${_first_ex}" "${_glfw_dir}")
                            file(REMOVE ${_zip_local})
                            message(STATUS "VENDOR: GLFW downloaded and extracted to ${_glfw_dir}")
                            set(_git_clone_res 0)
                        endif()
                    else()
                        # try with v prefix
                        set(_zip_url2 "https://github.com/glfw/glfw/archive/refs/tags/v${VENDOR_GLFW_GIT_TAG}.zip")
                        set(_zip_local2 "${CMAKE_BINARY_DIR}/glfw-v${VENDOR_GLFW_GIT_TAG}.zip")
                        file(DOWNLOAD ${_zip_url2} ${_zip_local2} SHOW_PROGRESS STATUS _dl_status2 LOG ERROR)
                        list(GET _dl_status2 0 _dl_code2)
                        if (_dl_code2 EQUAL 0)
                            file(ARCHIVE_EXTRACT INPUT ${_zip_local2} DESTINATION "${VENDOR_DIR}")
                            file(GLOB _ex_dirs2 RELATIVE "${VENDOR_DIR}" "${VENDOR_DIR}/glfw-*")
                            list(GET _ex_dirs2 0 _first_ex2)
                            if (EXISTS "${VENDOR_DIR}/${_first_ex2}")
                                file(RENAME "${VENDOR_DIR}/${_first_ex2}" "${_glfw_dir}")
                                file(REMOVE ${_zip_local2})
                                message(STATUS "VENDOR: GLFW downloaded and extracted to ${_glfw_dir}")
                                set(_git_clone_res 0)
                            endif()
                        else()
                            message(WARNING "VENDOR: failed to obtain GLFW via git or zip for tags '${VENDOR_GLFW_GIT_TAG}' and 'v${VENDOR_GLFW_GIT_TAG}'")
                        endif()
                    endif()
                endif()
            endif()
        endif()
    else()
        message(STATUS "VENDOR: using existing GLFW at ${_glfw_dir}")
        set(_git_clone_res 0)
    endif()

    if (EXISTS "${_glfw_dir}/CMakeLists.txt")
        add_subdirectory("${_glfw_dir}" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glfw_build")
    elseif (VENDOR_GLFW_FROM_GIT)
        message(STATUS "VENDOR: GLFW source not found in ${_glfw_dir}, attempting FetchContent...")
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG ${VENDOR_GLFW_GIT_TAG}
        )
        FetchContent_MakeAvailable(glfw)
    else()
        message(STATUS "VENDOR: GLFW not available (set VENDOR_GLFW_USE_CLONE=OFF and VENDOR_GLFW_FROM_GIT=ON to fetch)")
    endif()
else()
    # FetchContent path (if user prefers it)
    if (VENDOR_GLFW_FROM_GIT)
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG ${VENDOR_GLFW_GIT_TAG}
        )
        FetchContent_MakeAvailable(glfw)
    endif()
endif()

# GLEW detection: prefer CMake build provided by vendor/glew or thirdparty, else use prebuilt libs
set(GLEW_SEARCH_CANDIDATES
    "${VENDOR_DIR}/glew"
    "${CMAKE_SOURCE_DIR}/thirdparty/glew"
)
set(_glew_target_found OFF)
foreach(_gpath IN LISTS GLEW_SEARCH_CANDIDATES)
    if (EXISTS "${_gpath}/build/cmake/CMakeLists.txt")
        add_subdirectory("${_gpath}/build/cmake" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glew_build")
        # the GLEW CMake scripts create targets like glew_s or glew
        if (TARGET glew_s)
            set(_glew_target_found glew_s)
        elseif (TARGET glew)
            set(_glew_target_found glew)
        endif()
        break()
    endif()
endforeach()

# If not found via CMake, check for prebuilt libraries (MSVC layout)
if (NOT _glew_target_found)
    foreach(_gpath IN LISTS GLEW_SEARCH_CANDIDATES)
        if (EXISTS "${_gpath}/include")
            set(_glew_include_dir "${_gpath}/include")
            set(_glew_lib_x64 "${_gpath}/lib/Release/x64/glew32s.lib")
            set(_glew_lib_x64_dyn "${_gpath}/lib/Release/x64/glew32.lib")
            if (EXISTS "${_glew_lib_x64}")
                add_library(glew_s STATIC IMPORTED)
                set_target_properties(glew_s PROPERTIES IMPORTED_LOCATION "${_glew_lib_x64}")
                target_include_directories(glew_s INTERFACE "${_glew_include_dir}")
                set(_glew_target_found glew_s)
                break()
            elseif (EXISTS "${_glew_lib_x64_dyn}")
                add_library(glew IMPORTED)
                set_target_properties(glew PROPERTIES IMPORTED_LOCATION "${_glew_lib_x64_dyn}")
                target_include_directories(glew INTERFACE "${_glew_include_dir}")
                set(_glew_target_found glew)
                break()
            endif()
        endif()
    endforeach()
endif()

# --- Application target ---
add_executable(LevEngineApp ${CMAKE_SOURCE_DIR}/main.cpp)

# If GLEW wasn't configured as a target but headers exist in vendor/glew/include,
# expose that include directory to the application so <GL/glew.h> can be found.
# NOTE: add include dir regardless of whether GLEW created a target — linking
# may happen after includes are required during compile.
if (EXISTS "${VENDOR_DIR}/glew/include")
    target_include_directories(LevEngineApp PRIVATE "${VENDOR_DIR}/glew/include")
    message(STATUS "VENDOR: added ${VENDOR_DIR}/glew/include to LevEngineApp include paths")
endif()

# link vendor libraries if available
if (TARGET glfw)
    target_link_libraries(LevEngineApp PRIVATE glfw)
endif()
if (DEFINED _glew_target_found AND NOT _glew_target_found STREQUAL "OFF")
    target_link_libraries(LevEngineApp PRIVATE ${_glew_target_found})
endif()

# ensure app sees generated headers in build dir if any
target_include_directories(LevEngineApp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

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
