# Preamble 
cmake_minimum_required(VERSION 3.19)

project(CppChat)

# Project wide setup
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra")
set(CMAKE_CXX_EXTENSIONS NO)

# External dependencies 
add_subdirectory(dependencies)

# Register tests with cmake
include(CTest)
include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Catch2/extras/Catch.cmake)

# Main targets built for this project
add_subdirectory(src)

