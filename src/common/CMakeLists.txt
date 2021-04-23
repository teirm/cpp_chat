# add target library for the network common library
add_library(net_common
            STATIC 
            net_common.cpp)

# add library for common utilities
add_library(utilities_common
            STATIC
            utilities.cpp)

# specify the include directory for the 
# code
target_include_directories(net_common
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# specify include directory for utilities
target_include_directories(utilities_common
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# turn off GNU cxx extensions and place
# the library in the build/lib directory
set_target_properties(net_common
                      PROPERTIES CXX_EXTENSIONS OFF
                      CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_target_properties(utilities_common
                      PROPERTIES CXX_EXTENSIONS OFF
                      CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

