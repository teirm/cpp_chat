# add target library for the network common library
add_library(net_common
            STATIC 
            net_common.cpp)

# specify the include directory for the 
# code
target_include_directories(net_common
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# turn off GNU cxx extensions and place
# the library in the build/lib directory
set_target_properties(net_common
                      PROPERTIES CXX_EXTENSIONS OFF
                      CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
