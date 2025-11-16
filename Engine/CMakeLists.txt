cmake_minimum_required(VERSION 3.31.6)
# version
set(LevEngine_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(LevEngine_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(LevEngine_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(LevEngine_VERSION ${PROJECT_VERSION})
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h)

set(ENGINE_SOURCES
        Source/Core/Engine.cpp
        Source/Core/Engine.hpp
        Source/Core/EngineEvents.cpp
        Source/Core/EngineEvents.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h
)

add_library(${PROJECT_NAME}Lib STATIC ${ENGINE_SOURCES})
target_include_directories(${PROJECT_NAME}Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_BINARY_DIR})

# --- Vendor libraries (moved from top-level CMakeLists) ---
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

# Expose GLEW includes to consumers of the engine library if available
if (EXISTS "${VENDOR_DIR}/glew/include")
    target_include_directories(${PROJECT_NAME}Lib PUBLIC "${VENDOR_DIR}/glew/include")
    message(STATUS "VENDOR: added ${VENDOR_DIR}/glew/include to ${PROJECT_NAME}Lib include paths")
endif()

# GLFW: prefer a cloned copy under vendor/ or FetchContent fallback
option(VENDOR_GLFW_USE_CLONE "Clone GLFW into ${VENDOR_DIR}/glfw via git (safer)" ON)
option(VENDOR_GLFW_FROM_GIT "Use FetchContent to get GLFW from GitHub" OFF)
set(VENDOR_GLFW_GIT_TAG "3.3.8" CACHE STRING "GLFW git tag or commit to checkout when fetching GLFW")

if (VENDOR_GLFW_USE_CLONE)
    set(_glfw_dir "${VENDOR_DIR}/glfw")
    if (EXISTS "${_glfw_dir}/CMakeLists.txt")
        add_subdirectory("${_glfw_dir}" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glfw_build")
    elseif (VENDOR_GLFW_FROM_GIT)
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG ${VENDOR_GLFW_GIT_TAG}
        )
        FetchContent_MakeAvailable(glfw)
    else()
        message(STATUS "VENDOR: GLFW not available in ${_glfw_dir} (set VENDOR_GLFW_USE_CLONE=OFF and VENDOR_GLFW_FROM_GIT=ON to fetch)")
    endif()
else()
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

# Link vendor libraries to engine library if targets are present
if (TARGET glfw)
    target_link_libraries(${PROJECT_NAME}Lib PRIVATE glfw)
endif()

# GLEW: try to build as CMake subdir or use prebuilt/imported libs
set(GLEW_SEARCH_CANDIDATES
    "${VENDOR_DIR}/glew"
    "${CMAKE_SOURCE_DIR}/thirdparty/glew"
)
set(_glew_target_found OFF)
foreach(_gpath IN LISTS GLEW_SEARCH_CANDIDATES)
    if (EXISTS "${_gpath}/build/cmake/CMakeLists.txt")
        add_subdirectory("${_gpath}/build/cmake" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glew_build")
        if (TARGET glew_s)
            set(_glew_target_found glew_s)
        elseif (TARGET glew)
            set(_glew_target_found glew)
        endif()
        break()
    endif()
endforeach()

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
                add_library(glew STATIC IMPORTED)
                set_target_properties(glew PROPERTIES IMPORTED_LOCATION "${_glew_lib_x64_dyn}")
                target_include_directories(glew INTERFACE "${_glew_include_dir}")
                set(_glew_target_found glew)
                break()
            endif()
        endif()
    endforeach()
endif()

if (DEFINED _glew_target_found AND NOT _glew_target_found STREQUAL "OFF")
    target_link_libraries(${PROJECT_NAME}Lib PRIVATE ${_glew_target_found})
endif()

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
