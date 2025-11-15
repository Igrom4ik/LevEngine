# cmake/CPackReleaseConfig.cmake
# CPack configuration for creating a release ZIP containing only bin/, lib/ and README.md

# Package identity (can be overridden from command line via -D)
if (NOT DEFINED CPACK_PACKAGE_NAME)
    set(CPACK_PACKAGE_NAME "LevEngine" CACHE STRING "CPack package name")
endif ()

if (NOT DEFINED CPACK_PACKAGE_VERSION)
    if (DEFINED LEVENGINE_VERSION)
        set(CPACK_PACKAGE_VERSION "${LEVENGINE_VERSION}" CACHE STRING "CPack package version")
    elseif (DEFINED PROJECT_VERSION)
        set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}" CACHE STRING "CPack package version")
    else ()
        set(CPACK_PACKAGE_VERSION "1.0.0" CACHE STRING "CPack package version")
    endif ()
endif ()

# Use ZIP and TGZ generator by default
set(CPACK_GENERATOR "ZIP;TGZ" CACHE STRING "CPack generator")

# Ensure CMakeInstallPrefix exists — default to <binary>/install if not set
if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install prefix for packaging")
endif ()

# Output directory for produced archives (default: <build>/package)
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/package" CACHE PATH "CPack output directory")

# Do not wrap contents into an extra top-level dir
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF CACHE BOOL "Do not include top-level folder in archive")

# Only include bin/, lib/ and README.md from the install prefix
set(CPACK_INSTALLED_DIRECTORIES
        "${CMAKE_INSTALL_PREFIX}/bin;bin"
        "${CMAKE_INSTALL_PREFIX}/lib;lib"
        "${CMAKE_INSTALL_PREFIX}/README.md;."
)

# File name pattern: e.g. LevEngine-1.2.3.zip
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

# Basic metadata
set(CPACK_PACKAGE_VENDOR "LevEngine Team" CACHE STRING "CPack package vendor")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LevEngine release package" CACHE STRING "CPack description")

# Do not force component handling here
# set(CPACK_COMPONENTS_ALL ALL)

# Ignore patterns to exclude common source files from any source packaging
set(CPACK_SOURCE_IGNORE_FILES
    "/\\.git/"
    "/\\.gitignore$"
    "/\\.gitmodules$"
    "/\\.gitattributes$"
    "/\\.vs/"
    "/\\.vscode/"
    "/\\.idea/"
    ".*\\.cpp$"
    ".*\\.c$"
    ".*\\.h$"
    ".*\\.hpp$"
    "~$"
)
