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
    
    int wait(int timeout);
    int add(const io_mplex_fd_info_t fd_info);
    int add(std::vector<io_mplex_fd_info_t> fd_list);
    int remove(const int fd);
    int remove(std::vector<int> fd_list);
};
