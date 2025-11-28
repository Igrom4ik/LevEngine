# common options
set(APP_TARGET LENApp)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# version
set(LEN_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(LEN_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(LEN_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(LEN_VERSION ${PROJECT_VERSION})
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h)

# Generate application version header from template
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Source/Version.h.in")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Source/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
    list(APPEND APP_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
endif ()

set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

set(APP_SOURCES
        Source/main.cpp
        Source/Game.cpp
        Source/Game.hpp
        Source/TestObject.cpp
        Source/TestObject.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h
)

# Create application target from sources
add_executable(${APP_TARGET} ${APP_SOURCES} "Source/Game.cpp")

# Ensure MSVC Debug builds for the application use /ZI (Edit-and-Continue)
if (MSVC)
    target_compile_options(${APP_TARGET} PRIVATE
            $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/ZI>
    )
    target_link_options(${APP_TARGET} PRIVATE
            $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/DEBUG;/INCREMENTAL>
    )
endif ()

# Copy assets folder into target binary directory after build so runtime can access assets
if (EXISTS "${ASSETS_DIR}")
    add_custom_command(TARGET ${APP_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_TARGET}>/assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:${APP_TARGET}>/assets"
    )
endif ()

# Link engine library (engine target should export include dirs and dependencies)
target_link_libraries(${APP_TARGET} PRIVATE ${PROJECT_NAME}Lib)

# Also link GLFW directly into the application if the glfw target exists
if (TARGET glfw)
    message(STATUS "APP: Linking glfw into ${APP_TARGET}")
    target_link_libraries(${APP_TARGET} PRIVATE glfw)
endif ()

# If engine didn't provide GLEW targets, try system find_package as a fallback
if (NOT TARGET glew_s AND NOT TARGET glew)
    find_package(GLEW QUIET)
    if (TARGET GLEW::GLEW)
        target_link_libraries(${APP_TARGET} PRIVATE GLEW::GLEW)
    elseif (GLEW_FOUND)
        target_include_directories(${APP_TARGET} PRIVATE ${GLEW_INCLUDE_DIRS})
        target_link_libraries(${APP_TARGET} PRIVATE ${GLEW_LIBRARIES})
    else ()
        message(WARNING "GLEW not found for ${APP_TARGET}; engine should provide it or install system GLEW.")
    endif ()
endif ()

# If Tracy was added at top-level (external/tracy), link TracyClient into app and enable macro
if (DEFINED _TRACY_AVAILABLE AND _TRACY_AVAILABLE)
    if (TARGET TracyClient)
        message(STATUS "APP: Linking TracyClient into ${APP_TARGET} and enabling TRACY_ENABLE")
        target_link_libraries(${APP_TARGET} PRIVATE TracyClient)
        target_compile_definitions(${APP_TARGET} PRIVATE TRACY_ENABLE)
    else ()
        message(WARNING "APP: _TRACY_AVAILABLE set but TracyClient target not found. Ensure external/tracy provides TracyClient target.")
    endif ()
endif ()

target_include_directories(${APP_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_BINARY_DIR})

# folders
set_target_properties(${APP_TARGET} PROPERTIES FOLDER App)

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CmakeHelpers.cmake)
create_ide_folders(APP_SOURCES)
setup_precompiled_headers(${APP_TARGET} Source/pch.cpp Source/pch.hpp "${APP_SOURCES}")

# Optionally create a separate Tracy-enabled test build (LENAppTracy)
if (DEFINED BUILD_TRACY_TEST AND BUILD_TRACY_TEST)
    if (DEFINED _TRACY_AVAILABLE AND _TRACY_AVAILABLE)
        set(APP_TRACY_TARGET LENAppTracy)
        add_executable(${APP_TRACY_TARGET} ${APP_SOURCES} "Source/Game.cpp")

        # Same MSVC options as main app
        if (MSVC)
            target_compile_options(${APP_TRACY_TARGET} PRIVATE
                    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/ZI>
            )
            target_link_options(${APP_TRACY_TARGET} PRIVATE
                    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/DEBUG;/INCREMENTAL>
            )
        endif ()

        # Copy assets for tracy build as well
        if (EXISTS "${ASSETS_DIR}")
            add_custom_command(TARGET ${APP_TRACY_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_TRACY_TARGET}>/assets"
                    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:${APP_TRACY_TARGET}>/assets"
            )
        endif ()

        # Link engine library and optional libs
        target_link_libraries(${APP_TRACY_TARGET} PRIVATE ${PROJECT_NAME}Lib)
        if (TARGET glfw)
            target_link_libraries(${APP_TRACY_TARGET} PRIVATE glfw)
        endif ()

        if (NOT TARGET glew_s AND NOT TARGET glew)
            find_package(GLEW QUIET)
            if (TARGET GLEW::GLEW)
                target_link_libraries(${APP_TRACY_TARGET} PRIVATE GLEW::GLEW)
            elseif (GLEW_FOUND)
                target_include_directories(${APP_TRACY_TARGET} PRIVATE ${GLEW_INCLUDE_DIRS})
                target_link_libraries(${APP_TRACY_TARGET} PRIVATE ${GLEW_LIBRARIES})
            else ()
                message(WARNING "GLEW not found for ${APP_TRACY_TARGET}; engine should provide it or install system GLEW.")
            endif ()
        endif ()

        # Link Tracy and enable macro explicitly for the tracy test build
        if (TARGET TracyClient)
            target_link_libraries(${APP_TRACY_TARGET} PRIVATE TracyClient)
            target_compile_definitions(${APP_TRACY_TARGET} PRIVATE TRACY_ENABLE)
        else ()
            message(WARNING "APP: TracyClient not found while creating ${APP_TRACY_TARGET} target.")
        endif ()

        target_include_directories(${APP_TRACY_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_BINARY_DIR})
        set_target_properties(${APP_TRACY_TARGET} PROPERTIES FOLDER App)
        setup_precompiled_headers(${APP_TRACY_TARGET} Source/pch.cpp Source/pch.hpp "${APP_SOURCES}")
    else ()
        message(WARNING "BUILD_TRACY_TEST requested but Tracy not available (_TRACY_AVAILABLE=OFF)")
    endif ()
endif ()
