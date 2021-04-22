// utilities.cpp
//
// Utilities used throughout code
//
// 21 April 2021

// Implementation of Channel Class
#include <common/utilities.hpp>

#include <cassert>

#include <unistd.h>

Channel::Channel():
    read_pipe(0),
    write_pipe(0)
{
    int pipe_fd[2];
    int rc = pipe(pipe_fd);
    if (rc) {
        assert("Unable to create channel");
    }
    read_pipe = pipe_fd[0];
    write_pipe = pipe_fd[1];
}

Channel::~Channel()
{
    close(write_pipe);
    close(read_pipe);
}
