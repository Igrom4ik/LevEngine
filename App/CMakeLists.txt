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
target_include_directories(${APP_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_BINARY_DIR})

# folders
set_target_properties(${APP_TARGET} PROPERTIES FOLDER App)

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CmakeHelpers.cmake)
create_ide_folders(APP_SOURCES)
setup_precompiled_headers(${APP_TARGET} Source/pch.cpp Source/pch.hpp "${APP_SOURCES}")
