# version
set(LevEngine_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(LevEngine_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(LevEngine_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(LevEngine_VERSION ${PROJECT_VERSION})
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h)

set(ENGINE_SOURCES
        Source/Core/Engine.cpp
        Source/Core/Engine.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h
)

add_library(${PROJECT_NAME}Lib STATIC ${ENGINE_SOURCES})
target_include_directories(${PROJECT_NAME}Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_BINARY_DIR})

# folders
set_target_properties(${PROJECT_NAME}Lib PROPERTIES FOLDER Engine)

# Install library (only binaries). Headers are part of Source Code package and should not
# be included into the runtime binary distribution.
install(TARGETS ${PROJECT_NAME}Lib
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CmakeHelpers.cmake)
create_ide_folders(ENGINE_SOURCES)
