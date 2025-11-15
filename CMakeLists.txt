cmake_minimum_required(VERSION 3.31.6)

# Allow overriding project version via -DPROJECT_VERSION on command line or CI
set(PROJECT_VERSION "1.0.0" CACHE STRING "Project version (override with -DPROJECT_VERSION)")
project(LevEngine VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# Lint: detect any install(...) calls in the source tree that do not include DESTINATION.
# This prevents the "file INSTALL given no DESTINATION" error at install/package time.
# Skip vendor and build directories (these are external or generated).
if(NOT DEFINED SKIP_INSTALL_DESTINATION_LINT)
  message(STATUS "Running install() DESTINATION lint...")
  file(GLOB_RECURSE _cmake_files RELATIVE "${CMAKE_SOURCE_DIR}"
       "${CMAKE_SOURCE_DIR}/*.cmake" "${CMAKE_SOURCE_DIR}/**/CMakeLists.txt")
  set(_install_issues "")
  foreach(_f IN LISTS _cmake_files)
    string(REGEX MATCH "^(cmake-build|build_|build/|\.git/|_deps/|cmake-ci/|build_ci/|build_test/|cmake-build-).*" _skip "${_f}")
    if(_skip)
      continue()
    endif()
    # also skip vendor tree (third-party source) by default
    string(FIND "${_f}" "vendor/" _is_vendor)
    if(NOT "${_is_vendor}" STREQUAL "-1")
      continue()
    endif()

    file(READ "${CMAKE_SOURCE_DIR}/${_f}" _content)
    # collapse newlines so multi-line install(...) can be matched in a simple regex
    string(REGEX REPLACE "\r?\n" " " _single "${_content}")
    # Only match install(...) forms that require DESTINATION: FILES, TARGETS, PROGRAMS, DIRECTORY
    string(REGEX MATCHALL "install[[:space:]]*\\(\\s*(FILES|TARGETS|PROGRAMS|DIRECTORY) [^)]*\\)" _matches "${_single}")
    foreach(_m IN LISTS _matches)
      string(FIND "${_m}" "DESTINATION" _hasDest)
      if(_hasDest EQUAL -1)
        list(APPEND _install_issues "${_f}: ${_m}")
      endif()
    endforeach()
  endforeach()
  if(_install_issues)
    message(FATAL_ERROR "Found install() calls without DESTINATION (set SKIP_INSTALL_DESTINATION_LINT to skip):\n${_install_issues}")
  endif()
endif()

# Ensure a safe install prefix for packaging (use build/install by default to avoid requiring admin privileges)
if (NOT DEFINED CMAKE_INSTALL_PREFIX OR CMAKE_INSTALL_PREFIX STREQUAL "")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install prefix for packaging" FORCE)
endif()

# Disable thirdparty 'install' targets early so FetchContent or added subprojects do not install headers into global prefix
# These cache variables are read by glfw/glew CMakeLists to control installation behavior.
set(GLFW_INSTALL OFF CACHE BOOL "Do not install GLFW into global install prefix" FORCE)
set(GLEW_INSTALL OFF CACHE BOOL "Do not install GLEW into global install prefix" FORCE)

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
    source/pch.cpp
)

include_directories(source)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

# Precompiled headers (PCH) - use CMake's target_precompile_headers when available
# This improves build times by precompiling frequently used headers.
if (COMMAND target_precompile_headers)
  # target_precompile_headers requires modern CMake (>=3.16); check for the command availability
  target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/pch.hpp")
else()
  message(STATUS "target_precompile_headers not available; skipping PCH configuration")
endif()

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
            # Do not install GLFW headers/libraries into our install tree when using FetchContent
            # to keep release packages free of upstream source headers. The GLFW build supports GLFW_INSTALL option.
            set(GLFW_INSTALL OFF CACHE BOOL "Do not install GLFW into global install prefix" FORCE)
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
        # prevent vendor glfw from installing headers into system install prefix
        set(GLFW_INSTALL OFF CACHE BOOL "Do not install GLFW into global install prefix" FORCE)
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

# Generate a C/C++ header with version information if template exists
# Support both root-level Version.h.in and cmake/Version.h.in
if (EXISTS "${CMAKE_SOURCE_DIR}/Version.h.in")
    configure_file(${CMAKE_SOURCE_DIR}/Version.h.in ${CMAKE_BINARY_DIR}/App/Version.h @ONLY)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/App)
elseif (EXISTS "${CMAKE_SOURCE_DIR}/cmake/Version.h.in")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/Version.h.in ${CMAKE_BINARY_DIR}/App/Version.h @ONLY)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/App)
endif()

# Put build outputs to a single place
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Useful global properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# Install only runtime artifacts and package metadata (do not install source files)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION .)

# As a defensive measure, ensure README.md is present under the configured install prefix
# even if an alternate install invocation or generator changes DESTINATION semantics.
# Ensure CPACK_INSTALL_PREFIX is defined (fallback to CMAKE_INSTALL_PREFIX) so install(CODE) that
# references it does not emit a file(INSTALL DESTINATION "" ...) when CPACK isn't setting it.
if(NOT DEFINED CPACK_INSTALL_PREFIX OR "${CPACK_INSTALL_PREFIX}" STREQUAL "")
  set(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

install(CODE "
  if(NOT EXISTS \"${CPACK_INSTALL_PREFIX}/README.md\")\n    file(INSTALL DESTINATION \"${CPACK_INSTALL_PREFIX}\" FILES \"${CMAKE_SOURCE_DIR}/README.md\")\n  endif()\n")

# Optional release overrides
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/CPackReleaseConfig.cmake")
endif()

# CPack configuration (ensure README and package naming)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" CACHE STRING "CPack generator to use")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")

# Ensure CPack has minimum required metadata: package name and version
if(NOT DEFINED CPACK_PACKAGE_NAME)
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}" CACHE STRING "Package name for CPack")
endif()
# Prefer explicit PROJECT_VERSION (cache or -D override) which contains full semver; fall back to CMAKE_PROJECT_VERSION
if(DEFINED PROJECT_VERSION AND NOT "${PROJECT_VERSION}" STREQUAL "")
  set(_resolved_project_version "${PROJECT_VERSION}")
elseif(DEFINED CMAKE_PROJECT_VERSION AND NOT "${CMAKE_PROJECT_VERSION}" STREQUAL "")
  set(_resolved_project_version "${CMAKE_PROJECT_VERSION}")
else()
  set(_resolved_project_version "0.0.0")
endif()

# Diagnostic: print resolved project version during configuration
message(STATUS "Resolved project version for packaging: ${_resolved_project_version}")

if(NOT DEFINED CPACK_PACKAGE_VERSION)
  # Force CPACK_PACKAGE_VERSION from the configured PROJECT_VERSION-derived value
  set(CPACK_PACKAGE_VERSION "${_resolved_project_version}" CACHE STRING "Package version for CPack")
else()
  # Ensure CPACK_PACKAGE_VERSION reflects PROJECT_VERSION in case caller passed a value
  set(CPACK_PACKAGE_VERSION "${_resolved_project_version}" CACHE STRING "Package version for CPack" FORCE)
endif()

# Split version into components for CPACK (some generators use MAJOR/MINOR/PATCH fields)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ver_match ${CPACK_PACKAGE_VERSION})
if(_ver_match)
  # CMAKE_MATCH_1..3 are set by the previous REGEX MATCH call
  set(CPACK_PACKAGE_VERSION_MAJOR "${CMAKE_MATCH_1}")
  set(CPACK_PACKAGE_VERSION_MINOR "${CMAKE_MATCH_2}")
  set(CPACK_PACKAGE_VERSION_PATCH "${CMAKE_MATCH_3}")
else()
  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" _ver_match2 ${CPACK_PACKAGE_VERSION})
  if(_ver_match2)
    set(CPACK_PACKAGE_VERSION_MAJOR "${CMAKE_MATCH_1}")
    set(CPACK_PACKAGE_VERSION_MINOR "${CMAKE_MATCH_2}")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
  else()
    string(REGEX MATCH "^([0-9]+)" _ver_match3 ${CPACK_PACKAGE_VERSION})
    if(_ver_match3)
      set(CPACK_PACKAGE_VERSION_MAJOR "${CMAKE_MATCH_1}")
    else()
      set(CPACK_PACKAGE_VERSION_MAJOR "0")
    endif()
    set(CPACK_PACKAGE_VERSION_MINOR "0")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
  endif()
endif()

# Force CPACK_PACKAGE_FILE_NAME to use resolved values (use resolved_project_version directly to avoid truncation)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${_resolved_project_version}-${CMAKE_SYSTEM_NAME}" CACHE STRING "CPack output file name" FORCE)

# Ensure CPack packages only the installed runtime artifacts (bin, lib) and README
# Force this into the cache so that build-time generated CPack files honor it.
set(CPACK_INSTALLED_DIRECTORIES
    "${CPACK_INSTALL_PREFIX}/bin;bin"
    "${CPACK_INSTALL_PREFIX}/lib;lib"
    # Do not list README.md as a source path (absolute file paths with drive letters
    # can confuse CPack on Windows). Include the install prefix root so installed
    # files (including README.md copied by install(FILES ...)) are packaged.
    "${CPACK_INSTALL_PREFIX};."
    CACHE STRING "Directories to include in CPack package" FORCE)

# Explicitly set CPACK_SOURCE_IGNORE_FILES using character class for literal dot to avoid escape issues
set(CPACK_SOURCE_IGNORE_FILES
    "/[.]git/"
    "/[.]gitignore$"
    "/[.]gitmodules$"
    "/[.]gitattributes$"
    "/[.]vs/"
    "/[.]vscode/"
    "/[.]idea/"
    ".*[.]cpp$"
    ".*[.]c$"
    ".*[.]h$"
    ".*[.]hpp$"
    "~$"
    CACHE STRING "Files to ignore for CPack source packages" FORCE)

include(InstallRequiredSystemLibraries)
include(CPack)

# Helper targets
if (WIN32)
  add_custom_target(package-release
      COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target install
      # Clean up install tree to avoid packaging headers/docs from third-party libraries
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/include"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/share"
      COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target package
      COMMAND ${CMAKE_COMMAND} -E echo "Verifying produced package..."
      COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/scripts/check_package.ps1" "${CMAKE_BINARY_DIR}/package"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      COMMENT "Build, install and create release ZIP via CPack and verify package (Windows)")
else()
  add_custom_target(package-release
      COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target install
      # Clean up install tree to avoid packaging headers/docs from third-party libraries
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/include"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/share"
      COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target package
      COMMAND ${CMAKE_COMMAND} -E echo "Verifying produced package..."
      COMMAND "${CMAKE_SOURCE_DIR}/scripts/check_package.sh" "${CMAKE_BINARY_DIR}/package"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      COMMENT "Build, install and create release ZIP via CPack and verify (POSIX)")
endif()

# package-clean: install -> remove include/share -> create sanitized archive using scripts/pack_clean
if (WIN32)
  add_custom_target(package-clean
      COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target install
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/include"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/share"
      COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/scripts/pack_clean.ps1" "${CPACK_INSTALL_PREFIX}" "${CMAKE_BINARY_DIR}/package" "${CPACK_PACKAGE_FILE_NAME}-Windows.zip"
      COMMAND ${CMAKE_COMMAND} -E echo "Verifying produced package..."
      COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/scripts/check_package.ps1" "${CMAKE_BINARY_DIR}/package"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      COMMENT "Install, produce sanitized ZIP and verify (Windows)")
else()
  add_custom_target(package-clean
      COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config Release --target install
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/include"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${CPACK_INSTALL_PREFIX}/share"
      COMMAND "${CMAKE_SOURCE_DIR}/scripts/pack_clean.sh" "${CPACK_INSTALL_PREFIX}" "${CMAKE_BINARY_DIR}/package" "${CPACK_PACKAGE_FILE_NAME}-Linux.zip"
      COMMAND ${CMAKE_COMMAND} -E echo "Verifying produced package..."
      COMMAND "${CMAKE_SOURCE_DIR}/scripts/check_package.sh" "${CMAKE_BINARY_DIR}/package"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      COMMENT "Install, produce sanitized ZIP and verify (POSIX)")
endif()

add_custom_target(print-version
    COMMAND ${CMAKE_COMMAND} -E echo "LevEngine version: ${PROJECT_VERSION}"
    COMMENT "Print configured LevEngine version")

add_custom_target(package-release-help
    COMMAND ${CMAKE_COMMAND} -E echo "Run: cmake --build <build-dir> --target package-release --config Release"
    COMMENT "Show how to run package-release target")
