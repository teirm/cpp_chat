add_executable(test
               test.cpp)

set_target_properties(test 
                      PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(test
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(test PRIVATE parseflagscpp)
