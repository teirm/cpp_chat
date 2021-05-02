# add the subdirectory for the common library 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common)

# add the subdirectory for the io multiplexor library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/io_multiplexor)

# add subdirectory for the server code
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/server)
