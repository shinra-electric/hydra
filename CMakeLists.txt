cmake_minimum_required(VERSION 3.15...3.31)
set(CMAKE_POLICY_VERSION_MINIMUM 3.15)

set(FRONTEND "SDL3" CACHE STRING "Choose which frontend to use.")
option(MACOS_BUNDLE "Build a macOS app bundle" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hydra VERSION 0.1.0 LANGUAGES CXX)

# TODO: Change the build hash based on github
set(BUILD_HASH "Hash")

if (NOT CMAKE_BUILD_TYPE)
    message("Build Type not set, defaulting to Debug...")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Enable Objective-C and Swift on Apple platforms
if (APPLE)
    enable_language(OBJC OBJCXX Swift)
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Packages
find_package(fmt CONFIG REQUIRED)

# Externals
#add_subdirectory(externals/toml11) # Hatch already uses toml11
add_subdirectory(externals/dynarmic)
add_subdirectory(externals/cubeb)
#add_subdirectory(externals/luft)
add_subdirectory(externals/hatch)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include_directories(externals/metal-cpp externals/dynarmic externals/stb)

# TODO: find out why release builds are broken
# TODO: do compile flags need to be specified manually?
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
