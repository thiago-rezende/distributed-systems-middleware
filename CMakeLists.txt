# Minimum version to run the script
cmake_minimum_required(VERSION 3.16.0)

# Setting build type
set(DEFAULT_BUILD_TYPE "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Setting up the project name and version
project(DistributedSystemsMiddleware
        VERSION 1.0.0
        DESCRIPTION "Simple implementation of a middleware system"
        HOMEPAGE_URL "https://github.com/thiago-rezende/distributed-systems-middleware"
        LANGUAGES CXX)

# Add the cmake folder so the modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Conan Package Manager
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

# Setting the install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

# First for the generic no-config case (e.g. with mingw)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# Second, for multi-config builds (e.g. msvc)
foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string( TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach(OUTPUT_CONFIG CMAKE_CONFIGURATION_TYPES)

# Setting the path to the vendor folder
set(VENDOR ${CMAKE_SOURCE_DIR}/vendor)

# Spdlog options
option(SPDLOG_ENABLE_PCH "Build static or shared library using precompiled header to speed up compilation time" ON)

# JSON options
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

# Adding the vendor libs directories
add_subdirectory(${VENDOR}/catch2)
add_subdirectory(${VENDOR}/spdlog)
add_subdirectory(${VENDOR}/clipp)
add_subdirectory(${VENDOR}/json)

# Using Header Only
# add_subdirectory(${VENDOR}/websocketpp)

# Setting project options
option(USE_MSVC_DYNAMIC_LINKED_RUNTIME "Uses MSVC dynamic linked runtime" OFF)

# Generate an config.h file based on config.in file
configure_file(${CMAKE_SOURCE_DIR}/middleware/config.in ${CMAKE_SOURCE_DIR}/middleware/config.h @ONLY NEWLINE_STYLE LF)

# Setting the project folders
add_subdirectory(middleware)
add_subdirectory(agent)
add_subdirectory(client)

# Adding docs generation
add_subdirectory(docs)

# Adding Tests
enable_testing()
add_subdirectory(tests)

# Visual Studio Config
if(MSVC)
    ## Enable folders
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    ## Application
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT middleware)
    set_property(TARGET middleware PROPERTY FOLDER "Middleware")
    ## Utils
    set_property(TARGET copy_resources_middleware PROPERTY FOLDER "Utils")
    ## Tests
    set_property(TARGET middleware_tests PROPERTY FOLDER "Tests")
    ## Vendor
    # Google Test
    set_property(TARGET Catch2 PROPERTY FOLDER "Vendor/Catch2")
    set_property(TARGET Catch2WithMain PROPERTY FOLDER "Vendor/Catch2")
    # Spdlog
    set_property(TARGET spdlog PROPERTY FOLDER "Vendor/Spdlog")
endif(MSVC)
