// utilities.hpp
// 
// common utilities used throughout code
// 
// 10 April 2021

#pragma once

#include <cstring>

// memzero  zero out size bytes starting at addr
//
// @param[in]   addr    start address to zero out
// @param[in]   size    size to zero out
// 
// @return none
#define memzero(_addr_, _size_) memset((_addr_), 0, (_size_))


// Channel struct to provide RAII access to pipe
// handles in a cleaner manner
struct Channel {
    Channel();
    ~Channel();
    Channel(const Channel &rhs) = delete;
    Channel(Channel &&rhs) = delete;
    Channel& operator()(const Channel &rhs) = delete;

    int read_pipe;
    int write_pipe;
};
