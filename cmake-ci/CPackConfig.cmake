# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BINARY_7Z "OFF")
set(CPACK_BINARY_IFW "OFF")
set(CPACK_BINARY_INNOSETUP "OFF")
set(CPACK_BINARY_NSIS "ON")
set(CPACK_BINARY_NUGET "OFF")
set(CPACK_BINARY_WIX "OFF")
set(CPACK_BINARY_ZIP "OFF")
set(CPACK_BUILD_SOURCE_DIRS "C:/DEVOPS/LevEngine;C:/DEVOPS/LevEngine/cmake-ci")
set(CPACK_CMAKE_GENERATOR "Ninja")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake/share/cmake-4.1/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "LevEngine built using CMake")
set(CPACK_GENERATOR "NSIS")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "OFF")
set(CPACK_INNOSETUP_ARCHITECTURE "x64")
set(CPACK_INSTALL_CMAKE_PROJECTS "C:/DEVOPS/LevEngine/cmake-ci;LevEngine;ALL;/")
set(CPACK_INSTALL_PREFIX "C:/Program Files (x86)/LevEngine")
set(CPACK_MODULE_PATH "")
set(CPACK_NSIS_DISPLAY_NAME "LevEngine 1.0.0")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_PACKAGE_NAME "LevEngine 1.0.0")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OBJCOPY_EXECUTABLE "C:/msys64/ucrt64/bin/objcopy.exe")
set(CPACK_OBJDUMP_EXECUTABLE "C:/msys64/ucrt64/bin/objdump.exe")
set(CPACK_OUTPUT_CONFIG_FILE "C:/DEVOPS/LevEngine/cmake-ci/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake/share/cmake-4.1/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LevEngine built using CMake")
set(CPACK_PACKAGE_FILE_NAME "LevEngine-1.0.0-win64")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "LevEngine 1.0.0")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "LevEngine 1.0.0")
set(CPACK_PACKAGE_NAME "LevEngine")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Humanity")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PRODUCTBUILD_DOMAINS "ON")
set(CPACK_READELF_EXECUTABLE "C:/msys64/ucrt64/bin/readelf.exe")
set(CPACK_RESOURCE_FILE_LICENSE "C:/Program Files/CMake/share/cmake-4.1/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "C:/Program Files/CMake/share/cmake-4.1/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files/CMake/share/cmake-4.1/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_7Z "OFF")
set(CPACK_SOURCE_GENERATOR "")
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
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "C:/DEVOPS/LevEngine/cmake-ci/CPackSourceConfig.cmake")
set(CPACK_SOURCE_ZIP "OFF")
set(CPACK_SYSTEM_NAME "win64")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "win64")
set(CPACK_WIX_INSTALL_SCOPE "perMachine")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "C:/DEVOPS/LevEngine/cmake-ci/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
