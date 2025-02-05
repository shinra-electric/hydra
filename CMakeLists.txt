cmake_minimum_required(VERSION 3.15...3.31)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hydra VERSION 1.0 LANGUAGES CXX)

add_subdirectory(/Users/samuliak/Documents/nx-parser build/nx-parser) # TODO: dependencies directory

add_subdirectory(src)
