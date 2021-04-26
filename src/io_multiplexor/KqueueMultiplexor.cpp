// KqueueMultiplexor.cpp
//
// Implementation of KqueueMultiplexor class.
// Handles IO multiplexing using the Kqueue 
// interface on BSD based systems.
//
//

#include <common/log_util.hpp>
#include <io_multiplexor/KqueueMultiplexor.hpp>

#include <exception>

#include <unistd.h>
#include <sys/event.h>

// KqueueMultiplexor constructor
//
// Creates a new kqueue instance
//
// @throws std::runtime_error if unable to 
//         create kqueue
KqueueMultiplexor::KqueueMultiplexor() 
{
    sys_instance_fd_ = kqueue();
    if (sys_instance_fd_ == -1) {
        throw std::runtime_error("Unable to create kqueue");
    }
}

KqueueMultiplexor::~KqueueMultiplexor()
{
    if (close(sys_instance_fd_) == -1) {
        log(LogPriority::ERROR, "Failed to close kqueue\n");
    }
}

int KqueueMultiplexor::add(const io_mplex_fd_info_t &fd_info) {
    return -1;
}

int KqueueMultiplexor::add(const std::vector<io_mplex_fd_info_t> &fd_list)
{
    return -1;
}

int KqueueMultiplexor::add(std::vector<io_mplex_fd_info_t> &&fd_list)
{
    return add(fd_list);
}

int KqueueMultiplexor::remove(const int fd) 
{
    return -1;
}

int KqueueMultiplexor::remove(const std::vector<int> &fd_list) 
{

}

int KqueueMultiplexor::remove(std::vector<int> &&fd_list)
{
    return remove(fd_list);
}
