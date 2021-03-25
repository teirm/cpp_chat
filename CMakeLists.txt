# Preamble 
cmake_minimum_required(VERSION 3.19)

project(CppChat)

# Project wide setup
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Werror")
set(CMAKE_CXX_EXTENSIONS NO)

# External dependencies 
add_subdirectory(dependencies)

# Main targets built for this project
add_subdirectory(src)
