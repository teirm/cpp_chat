// EpollMultiplexor.cpp
//
// Implementation of EpollMultiplexor class
// Handles IO multiplexing using the Epoll
// interface on Linux based systems.
//

#include <common/log_util.hpp>
#include <io_multiplexor/EpollMultiplexor.hpp>

#include <exception>
#include <memory>

#include <unistd.h>
#include <sys/epoll.h>

using namespace std;

EpollMultiplexor::EpollMultiplexor(unsigned max_events):
    IoMultiplexor(max_events)
{
    // 
    instance_fd_ = epoll_create(max_events);
    if (instance_fd_ == -1) {
        throw std::runtime_error("Unable to create epoll");
    }
}

EpollMultiplexor::~EpollMultiplexor()
{
    if (close(instance_fd_) == -1) {
        log(LogPriority::ERROR, "Failed to close epoll\n");
    }
}

int EpollMultiplexor::wait(struct timespec *timeout, std::vector<io_mplex_fd_info_t> &events)
{
    struct epoll_event event_list[max_events_];
     
    int wait_seconds = 0;
    if (timeout != nullptr) {
        wait_seconds = timeout->tv_sec;
    }

    int n_events = epoll_wait(instance_fd_, event_list, max_events_, wait_seconds);  
    if (n_events == -1 || n_events == 0) {
        return n_events;
    }
    events.reserve(n_events);
    for (int i = 0; i < n_events; i++) {
    }
    return 0;
}

int EpollMultiplexor::add(const io_mplex_fd_info_t &fd_info) 
{
    int epoll_flags = flags_from_mplex(fd_info.flags);
    int epoll_filters = flags_from_mplex(fd_info.filters);
   
    struct epoll_event ev;
    ev.data.fd = fd_info.fd; 
    ev.events = epoll_flags | epoll_filters;

    int rc = epoll_ctl(instance_fd_, EPOLL_CTL_ADD, fd_info.fd, &ev);
    if (rc) {
        log(LogPriority::ERROR, "Unable to add to epoll instance\n");
    }
    return rc;
}

int EpollMultiplexor::add(const std::vector<io_mplex_fd_info_t> &fd_list)
{
    int rc = 0;
    int final_rc = 0; 
    for (auto& fd_info : fd_list) {
        rc = add(fd_info);
        if (rc) {
            final_rc = rc;
        }
    }
    return final_rc;
}

int EpollMultiplexor::add(std::vector<io_mplex_fd_info_t> &&fd_list)
{
    return add(fd_list);
}

int EpollMultiplexor::remove(const int fd) 
{
    return epoll_ctl(instance_fd_, EPOLL_CTL_DEL, fd, nullptr); 
}

int EpollMultiplexor::remove(const std::vector<int> &fd_list)
{
    int rc = 0;
    int final_rc = 0;
    for (int fd : fd_list) {
        rc = remove(fd);
        if (rc) {
            final_rc = rc;
        }
    }
    return final_rc;
}

int EpollMultiplexor::remove(std::vector<int> &&fd_list)
{
    return remove(fd_list);
}

int EpollMultiplexor::flags_from_mplex(io_mplex_flags_t mplex_values)
{
    int flags = 0;

    if (mplex_values & MPLEX_IN) {
        flags |= EPOLLIN;
    }
    if (mplex_values & MPLEX_OUT) {
        flags |= EPOLLOUT;
    }
    if (mplex_values & MPLEX_ONESHOT) {
        flags |= EPOLLONESHOT;
    }
    // epoll always waits for EPOLLHUP and EPOLLERR
    // so they are not checked for or added

    return flags;
}

io_mplex_flags_t EpollMultiplexor::flags_to_mplex(int epoll_values)
{
    io_mplex_flags_t flags = 0;
    if (epoll_values & EPOLLIN) {
        flags |= MPLEX_IN;
    }
    if (epoll_values & EPOLLOUT) {
        flags |= MPLEX_OUT;
    }
    if (epoll_values & EPOLLHUP) {
        flags |= MPLEX_EOF;
    }
    if (epoll_values & EPOLLERR) {
        flags |= MPLEX_ERR;
    }
    return flags;
}
