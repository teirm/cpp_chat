# Client CmakeLists

add_executable(test_prog test.cpp)

target_link_libraries(test_prog PRIVATE TermOx)
