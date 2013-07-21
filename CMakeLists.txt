if(WIN32 OR APPLE)
    cmake_minimum_required(VERSION 2.8.7)
else()
    cmake_minimum_required(VERSION 2.8)
endif()

set(AXR_CORE_SOURCE_DIR_REL "../core" CACHE STRING "Path to the AXR core sources (relative)")
set(AXR_CORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${AXR_CORE_SOURCE_DIR_REL})
if(NOT EXISTS "${AXR_CORE_SOURCE_DIR}" OR NOT IS_DIRECTORY "${AXR_CORE_SOURCE_DIR}")
    message(FATAL_ERROR "Could not find core module at expected path: ${AXR_CORE_SOURCE_DIR}")
endif()

set(AXR_COMMON_SOURCE_DIR_REL "../common" CACHE STRING "Path to the AXR common sources (relative)")
set(AXR_COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${AXR_COMMON_SOURCE_DIR_REL})
if(NOT EXISTS "${AXR_COMMON_SOURCE_DIR}" OR NOT IS_DIRECTORY "${AXR_COMMON_SOURCE_DIR}")
    message(FATAL_ERROR "Could not find common module at expected path: ${AXR_COMMON_SOURCE_DIR}")
endif()

set(AXR_FRAMEWORK_SOURCE_DIR_REL "../framework" CACHE STRING "Path to the AXR HSS framework sources (relative)")
set(AXR_FRAMEWORK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${AXR_FRAMEWORK_SOURCE_DIR_REL})
if(NOT EXISTS "${AXR_FRAMEWORK_SOURCE_DIR}" OR NOT IS_DIRECTORY "${AXR_FRAMEWORK_SOURCE_DIR}")
    message(FATAL_ERROR "Could not find framework module at expected path: ${AXR_FRAMEWORK_SOURCE_DIR}")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${AXR_CORE_SOURCE_DIR}/cmake ${AXR_CORE_SOURCE_DIR}/cmake/solar ${CMAKE_MODULE_PATH})
include(ForceOutBuild)
include(ForceLatinPaths)

project(AXRBrowser)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.5)

# Options
option(AXR_ALL_WARNINGS "Display as many warnings as possible" OFF)
option(AXR_BUILD_CORE "Builds the core library" ON)
set(AXR_BUILD_EXTENSIONS "QT" CACHE STRING "Builds extension libraries for toolkit integration")

if(BUILD_UNIVERSAL_BINS)
    set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
endif()

# Include utility modules
include(Utilities)
include(Solar)
include(FindDependencies)
include(CoreVariables)
include(Variables)
include(Package)
include(DeployQt4)

# This list is for OS X application bundles to copy in all necessary icons
file(GLOB axr_icon_SOURCES ${AXR_CORE_SOURCE_DIR}/share/icons/*.icns)

# The HSS framework, to be copied into application bundles
file(GLOB hss_SOURCES ${AXR_FRAMEWORK_SOURCE_DIR}/*)
list(REMOVE_ITEM hss_SOURCES ${AXR_FRAMEWORK_SOURCE_DIR}/.git)

# If we're using an IDE build system such as Visual Studio or Xcode,
# use fancier target names such as "Documentation" vs "docs"
string(FIND "${CMAKE_GENERATOR}" "Visual Studio" found_vs)
string(FIND "${CMAKE_GENERATOR}" "CodeBlocks" found_blocks)
if (found_vs GREATER -1 OR found_blocks GREATER -1 OR "${CMAKE_GENERATOR}" STREQUAL "Xcode")
    set(FANCY_TARGET_NAMES ON)
endif()

# Treat all external dependency headers as system headers so we don't see warnings for them
include_directories(SYSTEM ${SDL_INCLUDE_DIR} ${GTK2_INCLUDE_DIRS} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/src/firebreath)

include_directories_recursive(${AXR_CORE_SOURCE_DIR}/src/core)
include_directories_recursive(${AXR_CORE_SOURCE_DIR}/src/qt)
include_directories_recursive(${AXR_CORE_SOURCE_DIR}/src/TEST)

if(AXR_ALL_WARNINGS)
    if(MSVC)
        set_msvc_warning_level(4)
    endif()

    if(CMAKE_COMPILER_IS_CLANGXX OR CMAKE_COMPILER_IS_GNUCXX)
        set_gcc_warning_level("max")
    endif()

    if(CMAKE_COMPILER_IS_CLANGXX)
        set_clang_warning_level("max")
    endif()
endif()

if(AXR_NO_DEPRECATED_ENUMS)
    add_definitions(-DAXR_NO_DEPRECATED_ENUMS)
endif()

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

# Required to avoid "Symbol not found: __objc_empty_cache" from dyld
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lobjc")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lobjc")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lobjc")
endif()

if(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_REL bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_REL lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_REL lib)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_REL .)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_REL .)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_REL .)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
endif()

set(CPACK_OUTPUT_FILE_PREFIX dist)

# Set output directories for binaries and static and shared libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_RUNTIME_OUTPUT_DIRECTORY_REL})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_REL})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_LIBRARY_OUTPUT_DIRECTORY_REL})

list(APPEND ALL_LIBRARY_DIRS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
list(APPEND ALL_LIBRARY_DIRS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
list(APPEND ALL_LIBRARY_DIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
list(APPEND ALL_LIBRARY_DIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")

if(FANCY_TARGET_NAMES)
    set(TARGET_NAME_LIB_CORE Core)
    set(TARGET_NAME_LIB_QT "Qt integration")
    set(TARGET_NAME_LIB_COCOA "Cocoa integration")
    set(TARGET_NAME_APP_BROWSER Browser)
    set(TARGET_NAME_APP_COCOA "Cocoa example")
    set(TARGET_NAME_APP_SDL "SDL example")
else()
    set(TARGET_NAME_LIB_CORE core)
    set(TARGET_NAME_LIB_QT qt)
    set(TARGET_NAME_LIB_COCOA cocoa)
    set(TARGET_NAME_APP_BROWSER browser)
    set(TARGET_NAME_APP_COCOA cocoa_example)
    set(TARGET_NAME_APP_SDL sdl_example)
endif()

configure_file("${CMAKE_SOURCE_DIR}/config.in.h" "${CMAKE_BINARY_DIR}/config.h")

add_subdirectory("${AXR_CORE_SOURCE_DIR_REL}" "${CMAKE_BINARY_DIR}/_core")
add_subdirectory(src/browser)

if(PANDOC)
    if(UNIX)
        # Perform post-processing on CPack-generated packages
        add_custom_target(distribution
            COMMAND cmake --build . --target package && export pkgfilename="${CPACK_PACKAGE_FILE_NAME}" && ${CMAKE_SOURCE_DIR}/cmake/postpackage.sh
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    else()
        add_custom_target(distribution
            COMMAND cmake --build . --target package
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endif()

    include(InstallRequiredSystemLibraries)
    include(CPack)
endif()
