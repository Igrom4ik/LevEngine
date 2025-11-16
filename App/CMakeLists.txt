# common options
set(APP_TARGET LevEngineApp)

# version
set(LevEngine_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(LevEngine_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(LevEngine_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(LevEngine_VERSION ${PROJECT_VERSION})
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h)

# Generate application version header from template
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Source/Version.h.in")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Source/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
    list(APPEND APP_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
endif ()

set(APP_SOURCES
        Source/main.cpp
        Source/pch.cpp
        Source/pch.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h
)

# Append generated PVS preprocessed file only if it exists
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Source/main.PVS-Studio.i")
    list(APPEND APP_SOURCES "Source/main.PVS-Studio.i")
endif ()

add_executable(${APP_TARGET} ${APP_SOURCES})
target_link_libraries(${APP_TARGET} PRIVATE ${PROJECT_NAME}Lib)

# GLFW: prefer existing target from vendor (add_subdirectory) -> system package (config) -> headers fallback
if (TARGET glfw)
    target_link_libraries(${APP_TARGET} PRIVATE glfw)
else()
    # Try package config (glfw3Config.cmake / glfwTargets)
    find_package(glfw3 CONFIG QUIET)
    if (TARGET glfw::glfw)
        target_link_libraries(${APP_TARGET} PRIVATE glfw::glfw)
    else()
        # Try to locate headers and add include dir so compilation can proceed (linking may still fail)
        find_path(GLFW_INCLUDE_DIR "GLFW/glfw3.h"
                  HINTS "${VENDOR_DIR}/glfw/include" "${CMAKE_SOURCE_DIR}/vendor/glfw/include" "${CMAKE_INSTALL_PREFIX}/include")
        if (GLFW_INCLUDE_DIR)
            message(STATUS "Found GLFW headers in ${GLFW_INCLUDE_DIR}; adding include path for ${APP_TARGET}")
            target_include_directories(${APP_TARGET} PRIVATE ${GLFW_INCLUDE_DIR})
            # Optionally try to find a library to link (pkg-config or default name)
            find_library(GLFW_LIB NAMES glfw glfw3 PATHS "${CMAKE_INSTALL_PREFIX}/lib" NO_DEFAULT_PATH)
            if (GLFW_LIB)
                target_link_libraries(${APP_TARGET} PRIVATE ${GLFW_LIB})
            else()
                message(WARNING "GLFW headers found but library not located; linking may fail. Consider installing GLFW or enabling VENDOR_GLFW_FROM_GIT to fetch sources.")
            endif()
        else()
            message(FATAL_ERROR "GLFW not found. Ensure GLFW is available: place sources in ${VENDOR_DIR}/glfw, set -DVENDOR_GLFW_FROM_GIT=ON, or install system GLFW and make it discoverable.")
        endif()
    endif()
endif()

# GLEW: prefer vendor target -> try find_package -> headers fallback
if (TARGET glew_s)
    target_link_libraries(${APP_TARGET} PRIVATE glew_s)
elseif (TARGET glew)
    target_link_libraries(${APP_TARGET} PRIVATE glew)
else()
    # Try find_package for GLEW (config or module)
    find_package(GLEW QUIET)
    if (TARGET GLEW::GLEW)
        target_link_libraries(${APP_TARGET} PRIVATE GLEW::GLEW)
    elseif (GLEW_FOUND)
        target_include_directories(${APP_TARGET} PRIVATE ${GLEW_INCLUDE_DIRS})
        target_link_libraries(${APP_TARGET} PRIVATE ${GLEW_LIBRARIES})
    else()
        # Try headers fallback
        find_path(GLEW_INCLUDE_DIR "GL/glew.h"
                  HINTS "${VENDOR_DIR}/glew/include" "${CMAKE_INSTALL_PREFIX}/include")
        if (GLEW_INCLUDE_DIR)
            message(STATUS "Found GLEW headers in ${GLEW_INCLUDE_DIR}; adding include path for ${APP_TARGET}")
            target_include_directories(${APP_TARGET} PRIVATE ${GLEW_INCLUDE_DIR})
            find_library(GLEW_LIB NAMES glew glew32 glew_s PATHS "${CMAKE_INSTALL_PREFIX}/lib" NO_DEFAULT_PATH)
            if (GLEW_LIB)
                target_link_libraries(${APP_TARGET} PRIVATE ${GLEW_LIB})
            else()
                message(WARNING "GLEW headers found but library not located; linking may fail. Consider building vendor/glew or installing system GLEW.")
            endif()
        else()
            message(WARNING "GLEW not found; some OpenGL functionality may fail. Consider adding vendor/glew or installing system GLEW.")
        endif()
    endif()
endif()

target_include_directories(${APP_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_BINARY_DIR})

# folders
set_target_properties(${APP_TARGET} PROPERTIES FOLDER App)

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CmakeHelpers.cmake)
create_ide_folders(APP_SOURCES)
setup_precompiled_headers(${APP_TARGET} Source/pch.cpp Source/pch.hpp "${APP_SOURCES}")
