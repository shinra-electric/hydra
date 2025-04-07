cmake_minimum_required(VERSION 3.15...3.31)

option(CODESIGN_IDENTITY "The identity to use for macOS code signing.")

# TODO: use c++20
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hydra VERSION 1.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    message("Build Type not set, defaulting to Release...")
    set(CMAKE_BUILD_TYPE Release)
endif()

if (APPLE)
    enable_language(OBJC OBJCXX)
endif()

find_package(fmt CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

#add_subdirectory(externals/fmt)

#include_directories(externals/fmt/include)
include_directories(externals/metal-cpp)

# TODO: find out why release builds are broken
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    add_definitions(-DHYDRA_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
else ()
    message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif ()

add_subdirectory(src)
