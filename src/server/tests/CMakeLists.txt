# Cmake file for io_multiplexor_tests

add_executable(broadcaster_tests
               broadcaster_tests.cpp
               ../BroadCaster.cpp)

target_link_libraries(broadcaster_tests
                      PRIVATE Catch2::Catch2WithMain
                      PRIVATE io_mplex
                      PRIVATE net_common
                      PRIVATE utilities_common
                      PRIVATE pthread)

target_include_directories(broadcaster_tests
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

catch_discover_tests(broadcaster_tests)
