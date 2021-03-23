add_executable(test
               test.cpp)

set_target_properties(test 
                      PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(test PRIVATE parseflagscpp)
