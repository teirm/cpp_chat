# Cmake file for io_multiplexor_tests

add_executable(io_multiplexor_tests
               io_multiplexor_tests.cpp)

target_link_libraries(io_multiplexor_tests
                      PRIVATE Catch2::Catch2WithMain
                      PRIVATE io_mplex
                      PRIVATE utilities_common)

target_include_directories(io_multiplexor_tests
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

set_target_properties(io_multiplexor_tests
                      PROPERTIES CXX_EXTENSIONS OFF
                                 RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

catch_discover_tests(io_multiplexor_tests)
