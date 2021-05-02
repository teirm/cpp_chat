// EpollMultiplexor.hpp
//
// Concrete multiplexor class for epoll based
// io multiplexing on Linux.
//
// 25-April-2021

#include "IoMultiplexor.hpp"

class EpollMultiplexor final: public IoMultiplexor  {
public: 
    EpollMultiplexor();
    ~EpollMultiplexor();
    
    virtual int wait(struct timespec *timeout, std::vector<io_mplex_fd_info_t> &events) override;

    virtual int add(const io_mplex_fd_info_t &fd_info) override;
    virtual int add(const std::vector<io_mplex_fd_info_t> &fd_list) override;
    virtual int add(std::vector<io_mplex_fd_info_t> &&fd_list) override ;

    virtual int remove(const int fd) override;
    virtual int remove(const std::vector<int> &fd_list) override;
    virtual int remove(std::vector<int> &&fd_list) override;

private:
    int mplex_to_epoll(io_mplex_flags_t mplex_values);
};
