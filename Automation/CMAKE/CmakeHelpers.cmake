macro(system_info)
    message("=================== System info ===================")
    if (WIN32)
        message("Running on Windows")
    elseif (LINUX)
        message("Running on Linux")
    endif ()

    message("Using ${CMAKE_CXX_COMPILER_ID} compiler")
    if (MSVC)
        message("MSVC version: ${MSVC_VERSION}")
        message("MSVC toolset version: ${MSVC_TOOLSET_VERSION}")
    endif ()

    message("Compiler flags: ${CMAKE_CXX_FLAGS}")
    message("Compiler debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
    message("Compiler release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
    message("Configuration types: ${CMAKE_CONFIGURATION_TYPES}")
    message("===================================================")
endmacro()

function(setup_precompiled_headers TARGET PCH_SOURCE PCH_HEADER SOURCE_FILES)
    if (NOT BUILD_WITH_PCH)
        return()
    endif ()

    if (MSVC)
        # получим имя заголовка и полный путь к сгенерированному/исходному файлу
        get_filename_component(PCH_HEADER_NAME ${PCH_HEADER} NAME)
        # resolve header full path relative to target include dirs: prefer binary dir
        # Попытаемся найти файл в текущем binary dir, иначе используем указанное имя
        set(PCH_FULL_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PCH_HEADER_NAME}")
        if (EXISTS ${PCH_FULL_PATH})
            set(PCH_FORCING ${PCH_FULL_PATH})
        else ()
            # fall back to header as given
            set(PCH_FORCING ${PCH_HEADER})
        endif ()

        target_sources(${TARGET} PRIVATE ${PCH_HEADER} ${PCH_SOURCE})
        set_source_files_properties(${PCH_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc${PCH_HEADER_NAME}")

        foreach (SOURCE_FILE IN LISTS SOURCE_FILES)
            if (SOURCE_FILE MATCHES "\\.cpp$" AND NOT SOURCE_FILE STREQUAL ${PCH_SOURCE})
                set_source_files_properties(${SOURCE_FILE} PROPERTIES COMPILE_FLAGS "/Yu${PCH_HEADER_NAME}")
            endif ()
        endforeach ()

        # Use full path with /FI to force include the header from binary dir if generated there
        target_compile_options(${TARGET} PRIVATE "/FI${PCH_FORCING}")
    endif ()
endfunction()

macro(setup_conan HOST_PROFILE BUILD_PROFILE)
    set(CMAKE_CONAN_PATH "${CMAKE_BINARY_DIR}/conan_provider.cmake")
    if (NOT EXISTS ${CMAKE_CONAN_PATH})
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/refs/heads/develop2/conan_provider.cmake" "${CMAKE_CONAN_PATH}")
    endif ()

    set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_CONAN_PATH})
    set(CONAN_HOST_PROFILE "${HOST_PROFILE}")
    set(CONAN_BUILD_PROFILE "${BUILD_PROFILE}")
endmacro()

macro(create_ide_folders SOURCE_FILES)
    foreach (SOURCE IN LISTS ${SOURCE_FILES})
        get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
        string(REPLACE "${PROJECT_SOURCE_DIR}" "" SOURCE_PATH_REL "${SOURCE_PATH}")
        string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH_REL}")
        source_group("${GROUP_PATH}" FILES "${SOURCE}")
    endforeach ()
endmacro()

