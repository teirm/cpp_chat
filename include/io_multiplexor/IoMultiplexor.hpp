// IoMultiplexor.hpp
//
// Abstract virtual class for IoMultiplexing.
// Fronts concrete classes implementing system
// specific io multiplexing (e.g. epoll, kqueue, etc)
//
// 22-April-2021

#pragma once

#include <time.h>

#include <vector>
#include <utility>
#include <atomic>

typedef int io_mplex_flags_t;
enum IoMultiplexorFlags : io_mplex_flags_t 
{
   MPLEX_IN        = 0x01,
   MPLEX_OUT       = 0x02,
   MPLEX_ONESHOT   = 0x04,
   MPLEX_EOF       = 0x08,
   MPLEX_ERR       = 0x10 
};

struct io_mplex_fd_info_t {
    io_mplex_flags_t flags;
    io_mplex_flags_t filters;
    int fd;
};

class IoMultiplexor {
public:
    IoMultiplexor(unsigned max_events):
        instance_fd_(0),
        max_events_(max_events),
        n_events_(0) {}
    virtual ~IoMultiplexor() {}
    
    IoMultiplexor(const IoMultiplexor &rhs) = delete;
    IoMultiplexor& operator()(const IoMultiplexor& rhs) = delete;

    virtual unsigned get_events() const { return n_events_.load(); }

    virtual int wait(struct timespec *timeout, std::vector<io_mplex_fd_info_t> &events) = 0;
    
    virtual int add(const io_mplex_fd_info_t &fd_info)              = 0;
    virtual int add(const std::vector<io_mplex_fd_info_t> &fd_list) = 0;
    virtual int add(std::vector<io_mplex_fd_info_t> &&fd_list)      = 0;
   
    virtual int remove(const int fd)                                = 0;
    virtual int remove(const std::vector<int> &fd_list)             = 0;
    virtual int remove(std::vector<int> &&fd_list)                  = 0;

protected:
    virtual int flags_from_mplex(io_mplex_flags_t mplex_values) = 0;
    virtual io_mplex_flags_t flags_to_mplex(int flags) = 0; 

    // system specific file descriptor;
    int instance_fd_;
    unsigned max_events_;
    std::atomic<unsigned> n_events_;
};
