# Create libraries for multiplexing in a platform
# agnostic manner

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_library(io_mplex
                STATIC
                EpollMultiplexor.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    add_library(io_mplex
                STATIC
                KqueueMultiplexor.cpp)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# add include directories
target_include_directories(io_mplex
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# turn off GNU cxx extensions and place library in build/lib
set_target_properties(io_mplex
                      PROPERTIES CXX_EXTENSIONS OFF
                      CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
