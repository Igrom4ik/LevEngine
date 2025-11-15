# cmake/CPackReleaseConfig.cmake
# CPack configuration for creating a release ZIP (Windows only)

# Package identity
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

# Use ZIP generator for Windows
set(CPACK_GENERATOR "ZIP" CACHE STRING "CPack generator")

# ===== FIX: Do NOT override CPACK_INSTALL_PREFIX =====
# Let CPack use CMAKE_INSTALL_PREFIX from workflow

# ===== FIX: Disable DESTDIR mode =====
set(CPACK_SET_DESTDIR "OFF" CACHE BOOL "Do not use DESTDIR")

# Ensure CPack runs install from build tree
if(NOT DEFINED CPACK_INSTALL_CMAKE_PROJECTS)
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${PROJECT_NAME};ALL;Release" CACHE STRING "Projects to install for CPack")
endif()

# ===== FIX: Do NOT use CPACK_INSTALLED_DIRECTORIES =====
# Rely on install() rules from CMakeLists.txt only

# Metadata
set(CPACK_PACKAGE_VENDOR "LevEngine Team" CACHE STRING "CPack package vendor")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LevEngine release package" CACHE STRING "CPack description")

# Ignore source files in source packaging
set(CPACK_SOURCE_IGNORE_FILES
    "/[.]git/"
    "/[.]gitignore$"
    ".*[.]cpp$"
    ".*[.]c$"
    ".*[.]h$"
    ".*[.]hpp$"
    "~$"
)
