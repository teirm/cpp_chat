# Cmake file for server
add_executable(cpp_chat_server
               Server.cpp
               BroadCaster.cpp
               main.cpp)

# disable gnu C++ extensions
set_target_properties(cpp_chat_server
                      PROPERTIES  CXX_EXTENSIONS OFF
                                  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# specify include directory for server 
target_include_directories(cpp_chat_server
                           PRIVATE ${PROJECT_SOURCE_DIR}/include
                           PRIVATE ${PROJECT_SOURCE_DIR}/dependencies/ParseFlags/include)

# Add link libraries 
target_link_libraries(cpp_chat_server
                      PRIVATE pthread
                      PRIVATE net_common
                      PRIVATE utilities_common
                      PRIVATE parseflagscpp
                      PRIVATE io_mplex)

# add the subdirectory for tests
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
