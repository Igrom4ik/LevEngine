cmake_minimum_required(VERSION 3.31.6)
# version
set(LEN_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(LEN_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(LEN_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(LEN_VERSION ${PROJECT_VERSION})
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h)

# --- Early vendor detection: ensure GLFW subdirectory is added before engine target is created ---
# Allow overriding vendor directory from cache/preset
set(ENGINE_VENDOR_DIR "" CACHE PATH "Explicit path to Engine's vendor directory (overrides auto-detection)")

if (ENGINE_VENDOR_DIR)
    set(_VENDOR_DIR_EARLY "${ENGINE_VENDOR_DIR}")
elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
    set(_VENDOR_DIR_EARLY "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
else()
    set(_VENDOR_DIR_EARLY "${CMAKE_SOURCE_DIR}/vendor")
endif()

if (EXISTS "${_VENDOR_DIR_EARLY}/glfw/CMakeLists.txt")
    if (NOT TARGET glfw)
        message(STATUS "VENDOR-EARLY: adding GLFW subdirectory from ${_VENDOR_DIR_EARLY}/glfw before engine target creation")
        add_subdirectory("${_VENDOR_DIR_EARLY}/glfw" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glfw_build_early")
    else()
        message(STATUS "VENDOR-EARLY: glfw target already exists, skipping add_subdirectory")
    endif()
endif()

set(ENGINE_SOURCES
        Source/Core/Engine.cpp
        Source/Core/Engine.hpp
        Source/Core/Application.cpp
        Source/Core/Application.hpp
        Source/Core/input/InputManager.hpp
        Source/Core/input/InputManager.cpp
        Source/Core/input/InputKeys.hpp
        Source/Core/graphics/ShaderProgram.cpp
        Source/Core/graphics/ShaderProgram.hpp
        Source/Core/graphics/GraphicsAPI.cpp
        Source/Core/graphics/GraphicsAPI.hpp
        Source/Core/render/Material.cpp
        Source/Core/render/Material.hpp
        Source/Core/render/Mesh.cpp
        Source/Core/render/Mesh.hpp
        Source/Core/render/VertexLayout.hpp
        Source/Core/render/RenderQueue.cpp
        Source/Core/render/RenderQueue.hpp
        Source/Core/graphics/Colors.hpp
        Source/Core/scene/GameObject.cpp
        Source/Core/scene/GameObject.hpp
        Source/Core/scene/Scene.cpp
        Source/Core/scene/Scene.hpp
        Source/Core/scene/Component.cpp
        Source/Core/scene/Component.hpp
        Source/Core/scene/components/MeshComponent.cpp
        Source/Core/scene/components/MeshComponent.hpp

        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.h
        Source/Core/scene/Component.cpp
        Source/Core/scene/Component.hpp
        Source/Core/scene/components/MeshComponent.cpp
        Source/Core/scene/components/MeshComponent.hpp
)

add_library(${PROJECT_NAME}Lib STATIC ${ENGINE_SOURCES})
# Ensure MSVC Debug builds for this target use /ZI (Edit-and-Continue)
if (MSVC)
    target_compile_options(${PROJECT_NAME}Lib PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/ZI>
    )
    target_link_options(${PROJECT_NAME}Lib PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/DEBUG;/INCREMENTAL>
    )

    # Also set Visual Studio specific property for Edit-and-Continue (helps VS generators)
    set_target_properties(${PROJECT_NAME}Lib PROPERTIES VS_DEBUG_INFORMATION_FORMAT "EditAndContinue")
endif()

target_include_directories(${PROJECT_NAME}Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_BINARY_DIR})

# --- Vendor libraries (moved from top-level CMakeLists) ---
# Allow overriding vendor directory from cache/preset
set(ENGINE_VENDOR_DIR "" CACHE PATH "Explicit path to Engine's vendor directory (overrides auto-detection)")

if (ENGINE_VENDOR_DIR)
    set(VENDOR_DIR "${ENGINE_VENDOR_DIR}")
elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
    set(VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
else()
    set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
endif()

message(STATUS "VENDOR: using VENDOR_DIR=${VENDOR_DIR} (ENGINE_VENDOR_DIR=${ENGINE_VENDOR_DIR})")

# Option: don't perform network fetches during configuration by default (prevents VS hang)
option(ENGINE_ALLOW_FETCHCONTENT "Allow CMake to fetch vendor dependencies from network (OFF recommended for IDEs)" OFF)
message(STATUS "VENDOR: ENGINE_ALLOW_FETCHCONTENT=${ENGINE_ALLOW_FETCHCONTENT}")

# Add directory-level include for glm if present (ensures headers visible to all targets)
# NOTE: avoid global include_directories which can interfere with system include order in some toolchains.
if (EXISTS "${VENDOR_DIR}/glm")
    message(STATUS "VENDOR: glm folder detected at ${VENDOR_DIR}/glm; will add to target includes (no global include_directories)")
    # Do not call include_directories() globally; use target_include_directories below so ordering is controlled per-target
endif()

# Ensure GLM headers from vendor are visible (handle official repo layout vendor/glm/glm and fallback vendor/glm)
if (EXISTS "${VENDOR_DIR}/glm/glm")
    message(STATUS "VENDOR: exposing include dir ${VENDOR_DIR}/glm/glm for GLM headers (glm/glm layout)")
    target_include_directories(${PROJECT_NAME}Lib PUBLIC "${VENDOR_DIR}/glm/glm")
elseif (EXISTS "${VENDOR_DIR}/glm")
    message(STATUS "VENDOR: exposing include dir ${VENDOR_DIR}/glm for GLM headers (generic fallback)")
    target_include_directories(${PROJECT_NAME}Lib PUBLIC "${VENDOR_DIR}/glm")
endif()

# GLM (header-only math library)
# Try system glm, then prefer official vendor submodule layout (vendor/glm), then FetchContent fallback
find_package(glm QUIET)
if (TARGET glm::glm)
    message(STATUS "VENDOR: Found GLM package; linking glm::glm")
    target_link_libraries(${PROJECT_NAME}Lib PUBLIC glm::glm)
else()
    if (EXISTS "${VENDOR_DIR}/glm/CMakeLists.txt")
        message(STATUS "VENDOR: adding glm subdirectory from ${VENDOR_DIR}/glm")
        add_subdirectory("${VENDOR_DIR}/glm" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glm_build")
        if (TARGET glm::glm)
            message(STATUS "VENDOR: linked glm::glm from vendor submodule")
            target_link_libraries(${PROJECT_NAME}Lib PUBLIC glm::glm)
        endif()
    else()
        message(STATUS "VENDOR: glm not found as a CMake subdir; will rely on exposed include dirs or FetchContent fallback")
        if (ENGINE_ALLOW_FETCHCONTENT)
            include(FetchContent)
            FetchContent_Declare(
                glm
                GIT_REPOSITORY https://github.com/g-truc/glm.git
                GIT_TAG 1.0.2
            )
            FetchContent_MakeAvailable(glm)
            if (TARGET glm::glm)
                message(STATUS "VENDOR: FetchContent provided glm; linking glm::glm")
                target_link_libraries(${PROJECT_NAME}Lib PUBLIC glm::glm)
            endif()
        else()
            message(STATUS "VENDOR: FetchContent disabled for glm (ENGINE_ALLOW_FETCHCONTENT=OFF). If headers missing, set ENGINE_ALLOW_FETCHCONTENT=ON or provide vendor/glm locally.")
        endif()
    endif()
endif()

# Expose GLEW includes to consumers of the engine library if available
if (EXISTS "${VENDOR_DIR}/glew/include")
    target_include_directories(${PROJECT_NAME}Lib PUBLIC "${VENDOR_DIR}/glew/include")
    message(STATUS "VENDOR: added ${VENDOR_DIR}/glew/include to ${PROJECT_NAME}Lib include paths")
else()
    message(STATUS "VENDOR: glew include dir not found at ${VENDOR_DIR}/glew/include")
endif()

# GLFW: prefer a cloned copy under vendor/ or FetchContent fallback
option(VENDOR_GLFW_USE_CLONE "Clone GLFW into ${VENDOR_DIR}/glfw via git (safer)" ON)
option(VENDOR_GLFW_FROM_GIT "Use FetchContent to get GLFW from GitHub" OFF)
set(VENDOR_GLFW_GIT_TAG "3.3.8" CACHE STRING "GLFW git tag or commit to checkout when fetching GLFW")

if (VENDOR_GLFW_USE_CLONE)
    set(_glfw_dir "${VENDOR_DIR}/glfw")
    message(STATUS "VENDOR: checking for GLFW in ${_glfw_dir}")
    if (EXISTS "${_glfw_dir}/CMakeLists.txt")
        if (NOT TARGET glfw)
            message(STATUS "VENDOR: adding GLFW subdirectory from ${_glfw_dir}")
            add_subdirectory("${_glfw_dir}" "${CMAKE_CURRENT_BINARY_DIR}/vendor_glfw_build")
        else()
            message(STATUS "VENDOR: glfw target already exists, skipping add_subdirectory")
        endif()
    elseif (VENDOR_GLFW_FROM_GIT AND ENGINE_ALLOW_FETCHCONTENT)
        message(STATUS "VENDOR: GLFW not found locally, will FetchContent")
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG ${VENDOR_GLFW_GIT_TAG}
        )
        FetchContent_MakeAvailable(glfw)
    else()
        message(STATUS "VENDOR: GLFW not available in ${_glfw_dir} (set VENDOR_GLFW_USE_CLONE=OFF and VENDOR_GLFW_FROM_GIT=ON and ENGINE_ALLOW_FETCHCONTENT=ON to fetch)")
    endif()
else()
    if (VENDOR_GLFW_FROM_GIT AND ENGINE_ALLOW_FETCHCONTENT)
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG ${VENDOR_GLFW_GIT_TAG}
        )
        FetchContent_MakeAvailable(glfw)
    endif()
endif()

# If GLFW headers exist in vendor folder expose them so includes like <GLFW/glfw3.h> resolve
if (EXISTS "${VENDOR_DIR}/glfw/include")
    message(STATUS "VENDOR: exposing include dir ${VENDOR_DIR}/glfw/include for GLFW headers")
    target_include_directories(${PROJECT_NAME}Lib PUBLIC "${VENDOR_DIR}/glfw/include")
endif()

# Link vendor libraries to engine library if targets are present
if (TARGET glfw)
    message(STATUS "VENDOR: target 'glfw' exists; linking to ${PROJECT_NAME}Lib")
    target_link_libraries(${PROJECT_NAME}Lib PUBLIC glfw)
else()
    message(STATUS "VENDOR: target 'glfw' not found; GLFW will not be linked")
endif()

# GLEW: try to build as CMake subdir or use prebuilt/imported libs
set(GLEW_SEARCH_CANDIDATES
    "${VENDOR_DIR}/glew"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glew"
    "${CMAKE_SOURCE_DIR}/thirdparty/glew"
    "${CMAKE_SOURCE_DIR}/thirdparty"
)
set(_glew_target_found OFF)
foreach(_gpath IN LISTS GLEW_SEARCH_CANDIDATES)
    if (EXISTS "${_gpath}/build/cmake/CMakeLists.txt")
        message(STATUS "VENDOR: found glew cmake in ${_gpath}/build/cmake, adding subdirectory")
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
            message(STATUS "VENDOR: found glew include in ${_gpath}/include")
            set(_glew_include_dir "${_gpath}/include")
            set(_glew_lib_x64 "${_gpath}/lib/Release/x64/glew32s.lib")
            set(_glew_lib_x64_dyn "${_gpath}/lib/Release/x64/glew32.lib")
            if (EXISTS "${_glew_lib_x64}")
                message(STATUS "VENDOR: found static glew lib at ${_glew_lib_x64}")
                add_library(glew_s STATIC IMPORTED)
                set_target_properties(glew_s PROPERTIES IMPORTED_LOCATION "${_glew_lib_x64}")
                target_include_directories(glew_s INTERFACE "${_glew_include_dir}")
                set(_glew_target_found glew_s)
                break()
            elseif (EXISTS "${_glew_lib_x64_dyn}")
                message(STATUS "VENDOR: found shared glew lib at ${_glew_lib_x64_dyn}")
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
    message(STATUS "VENDOR: linking ${_glew_target_found} into ${PROJECT_NAME}Lib")
    target_link_libraries(${PROJECT_NAME}Lib PRIVATE ${_glew_target_found})
else()
    message(STATUS "VENDOR: no GLEW target found; GLEW will not be linked")
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
