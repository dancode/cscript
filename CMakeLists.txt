cmake_minimum_required(VERSION 3.10)
project(cscript C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Set default build options based on the operating system
if(WIN32)
    set(CSCRIPT_BUILD_WINDOWS_DEFAULT ON)
    set(CSCRIPT_BUILD_POSIX_DEFAULT OFF)
else()
    set(CSCRIPT_BUILD_WINDOWS_DEFAULT OFF)
    set(CSCRIPT_BUILD_POSIX_DEFAULT ON)
endif()

option(CSCRIPT_BUILD_WINDOWS "Build for Windows" ${CSCRIPT_BUILD_WINDOWS_DEFAULT})
option(CSCRIPT_BUILD_POSIX "Build for POSIX" ${CSCRIPT_BUILD_POSIX_DEFAULT})

if(CSCRIPT_BUILD_WINDOWS AND CSCRIPT_BUILD_POSIX)
    message(FATAL_ERROR "Both CSCRIPT_BUILD_WINDOWS and CSCRIPT_BUILD_POSIX are ON. Please choose one.")
endif()

if(NOT CSCRIPT_BUILD_WINDOWS AND NOT CSCRIPT_BUILD_POSIX)
    message(FATAL_ERROR "At least one of CSCRIPT_BUILD_WINDOWS or CSCRIPT_BUILD_POSIX must be ON.")
endif()

# Set output directories for binaries and libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Core Library: cscript ---
# This static library contains the core runtime used by the host application.
add_library(cscript STATIC)

target_include_directories(cscript PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(CSCRIPT_CORE_SOURCES
    source/cscript.c
    source/cscript_log.c
)

if(CSCRIPT_BUILD_WINDOWS)
    list(APPEND CSCRIPT_CORE_SOURCES
        source/cscript_platform_win.c
        source/cscript_watcher_win.c
    )
    target_compile_definitions(cscript PUBLIC CSCRIPT_PLATFORM_WINDOWS=1)
elseif(CSCRIPT_BUILD_POSIX)
    message(WARNING "POSIX platform is not fully implemented and will use stubs.")
    list(APPEND CSCRIPT_CORE_SOURCES
        source/cscript_platform_posix.c
        source/cscript_watcher_posix.c
    )
    target_compile_definitions(cscript PUBLIC CSCRIPT_PLATFORM_POSIX=1)
endif()

target_sources(cscript PRIVATE ${CSCRIPT_CORE_SOURCES})

# --- Example Script: example_script (Shared Library) ---
# This is the dynamically loaded script module that can be hot-reloaded.
add_library(example_script SHARED
    examples/script/example_script.c
)
target_include_directories(example_script PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
# Define CSCRIPT_EXPORTS so that cscript_export.h uses __declspec(dllexport)
target_compile_definitions(example_script PRIVATE CSCRIPT_EXPORTS)
# Place the shared library in the same directory as the host executable
set_target_properties(example_script PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


# --- Example Host: cscript_host (Executable) ---
add_executable(cscript_host
    examples/host/main.c
)
target_link_libraries(cscript_host PRIVATE cscript)
target_include_directories(cscript_host PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
