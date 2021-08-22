// utilities.cpp
//
// Utilities used throughout code
//
// 21 April 2021

// Implementation of Channel Class
#include <common/utilities.hpp>

#include <cassert>
#include <stdexcept>

#include <unistd.h>

Channel::Channel():
    read_pipe(0),
    write_pipe(0)
{
    int pipe_fd[2];
    int rc = pipe(pipe_fd);
    if (rc) {
        std::runtime_error("Unable to create channel");
    }
    read_pipe = pipe_fd[0];
    write_pipe = pipe_fd[1];
}

Channel::~Channel()
{
    close(write_pipe);
    close(read_pipe);
}

// Write the contents of msg to the channel
//
// @param[in]   msg     message to write to channel
//
// @return  number of bytes written
//          -1 on error
auto Channel::write(const std::string &msg) -> int
{
    return ::write(get_write_end(), msg.c_str(), msg.size());
}
