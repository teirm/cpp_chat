// IoMultiplexor.hpp
//
// Abstract virtual class for IoMultiplexing.
// Fronts concrete classes implementing system
// specific io multiplexing (e.g. epoll, kqueue, etc)
//
// 22-April-2021

#include <vector>
#include <utility>
#include <atomic>

typedef unsigned int io_mplex_flags_t;
enum class IoMultiplexorFlags : io_mplex_flags_t {
    MPLEX_IN        = 0x1,
    MPLEX_OUT       = 0x2,
    MPLEX_ONESHOT   = 0x4,
    MPLEX_EOF       = 0x8
};

typedef std::pair<int, io_mplex_flags_t> io_mplex_fd_info_t;

class IoMultiplexor {
public:
    IoMultiplexor():
        sys_instance_fd_(0),
        n_events_(0) {}
    virtual ~IoMultiplexor() {}
    
    IoMultiplexor(const IoMultiplexor &rhs) = delete;
    IoMultiplexor& operator()(const IoMultiplexor& rhs) = delete;

    virtual int wait(int timeout)                                   = 0;
    virtual int add(const io_mplex_fd_info_t &fd_info)              = 0;
    virtual int add(const std::vector<io_mplex_fd_info_t> &fd_list) = 0;
    virtual int add(std::vector<io_mplex_fd_info_t> &&fd_list)      = 0;
    virtual int remove(const int fd)                                = 0;
    virtual int remove(const std::vector<int> &fd_list)             = 0;
    virtual int remove(std::vector<int> &&fd_list)                  = 0;

protected:
    // system specific file descriptor;
    int sys_instance_fd_;
    std::atomic<int> n_events_;
};
