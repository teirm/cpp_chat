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
#include <memory>

#include <unistd.h>
#include <sys/event.h>

using namespace std;

// KqueueMultiplexor constructor
//
// Creates a new kqueue instance
//
// @throws std::runtime_error if unable to 
//         create kqueue
KqueueMultiplexor::KqueueMultiplexor() 
{
    instance_fd_ = kqueue();
    if (sys_instance_fd_ == -1) {
        throw std::runtime_error("Unable to create kqueue");
    }
}

KqueueMultiplexor::~KqueueMultiplexor()
{
    if (close(instance_fd_) == -1) {
        log(LogPriority::ERROR, "Failed to close kqueue\n");
    }
}

int KqueueMultiplexor::add(const io_mplex_fd_info_t &fd_info) 
{
    int flags = mplex_to_kqueue(fd_info.flags);
    int filters = mplex_to_kqueue(fd_info.filters);
    
    struct kevent event;
    EV_SET(&event, fd_info.fd, filters, flags, 0, 0, nullptr);
    return kevent(instance_fd_, &event, 1, nullptr, 0, nullptr);
}

int KqueueMultiplexor::add(const std::vector<io_mplex_fd_info_t> &fd_list)
{
    size_t size = fd_list.size();
    auto change_list = unique_ptr<struct kevent []>(new struct kevent[size]);
    int index = 0;
    int flags = 0;
    int filters = 0;
    for (auto &fd_info : fd_list) {
        flags = mplex_to_kqueue(fd_info.flags);
        filters = mplex_to_kqueue(fd_info.filters);
        EV_SET(&change_list[index], fd_info.fd, filters, flags, 0, 0, nullptr);
    }
    return kevent(instance_fd_, change_list.get(), size, nullptr, 0, nullptr); 
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

int KqueueMultiplexor::mplex_to_kqueue(io_mplex_flags_t mplex_values)
{
    int flags = 0;

    if (mplex_values & MPLEX_IN) {
        flags |= EVFILT_READ;
    } 
    if (mplex_values & MPLEX_OUT) {
        flags |= EVFILT_WRITE;
    }
    if (mplex_values & MPLEX_ONESHOT) {
        flags |= EV_ONESHOT;
    }
    if (mplex_values & MPLEX_EOF) {
        flags |= EV_EOF;
    }
    return flags;
}
