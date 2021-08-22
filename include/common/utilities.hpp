// utilities.hpp
// 
// common utilities used throughout code
// 
// 10 April 2021

#pragma once

#include <cstring>
#include <string>

// memzero  zero out size bytes starting at addr
//
// @param[in]   addr    start address to zero out
// @param[in]   size    size to zero out
// 
// @return none
#define memzero(_addr_, _size_) memset((_addr_), 0, (_size_))


// Channel struct to provide RAII access to pipe
// handles in a cleaner manner
class Channel {
public:
    Channel();
    ~Channel();
    Channel(const Channel &rhs) = delete;
    Channel(Channel &&rhs) = delete;
    Channel& operator()(const Channel &rhs) = delete;
    
    auto get_read_end() -> int { return read_pipe; }
    auto get_write_end() -> int { return write_pipe; }
    auto write(const std::string &msg) -> int;

private:
    int read_pipe;
    int write_pipe;
};
