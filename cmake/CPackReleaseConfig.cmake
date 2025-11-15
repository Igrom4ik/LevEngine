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

# Do NOT override CMAKE_INSTALL_PREFIX here — overriding the canonical install prefix can lead to
# doubled/absolute paths when CPack composes temporary packaging directories. Use a dedicated
# CPACK_INSTALL_PREFIX (staging directory under the build tree) for packaging operations instead.
if (NOT DEFINED CPACK_INSTALL_PREFIX)
    set(CPACK_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "CPack install prefix (override to control where project is installed during packaging)")
endif()

# Ensure DESTDIR-style behavior so install goes into CPACK_INSTALL_PREFIX rather than system locations
set(CPACK_SET_DESTDIR "ON" CACHE BOOL "Use DESTDIR for packaging installs")

# Ensure CPack will run install from the build tree when packaging
# Format: <build-dir>;<project-name>;<component-to-install>;<config>
if(NOT DEFINED CPACK_INSTALL_CMAKE_PROJECTS)
    # Use Release config by default for single-config packaging; CPack may override with -C
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${PROJECT_NAME};ALL;Release" CACHE STRING "Projects to install for CPack")
endif()

# ===== FIX: Do NOT use CPACK_INSTALLED_DIRECTORIES with absolute paths =====
# On Windows, CPack cannot handle absolute paths in CPACK_INSTALLED_DIRECTORIES.
# Instead, rely on install() rules from CMakeLists.txt and CPACK_INSTALL_CMAKE_PROJECTS.
# CPack will automatically include files installed via install(FILES/TARGETS/DIRECTORY/...).
# If you previously attempted to include absolute paths like:
#   "${CPACK_INSTALL_PREFIX}/bin;bin"
# then that can cause CPack to construct invalid temporary paths.
# Therefore this project intentionally does NOT set CPACK_INSTALLED_DIRECTORIES here.

# File name pattern: e.g. LevEngine-1.2.3.zip
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

# Basic metadata
set(CPACK_PACKAGE_VENDOR "LevEngine Team" CACHE STRING "CPack package vendor")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LevEngine release package" CACHE STRING "CPack description")

# Do not force component handling here
# set(CPACK_COMPONENTS_ALL ALL)

# Ignore patterns to exclude common source files from any source packaging
# Use [.] for a literal dot to avoid ambiguous backslash escaping in generated CMake files
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
)
