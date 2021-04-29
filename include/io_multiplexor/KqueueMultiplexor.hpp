// KqueueMultiplexor.hpp
//
// Concrete class for kqueue based io multiplexing
// on BSD based systems
//
// 25-April-2021

#include "IoMultiplexor.hpp"

#include <utility>

class KqueueMultiplexor final: public IoMultiplexor  {
public: 
    KqueueMultiplexor();
    ~KqueueMultiplexor();
    
    int wait(int timeout);
    int add(const io_mplex_fd_info_t &fd_info);
    int add(const std::vector<io_mplex_fd_info_t> &fd_list);
    int add(std::vector<io_mplex_fd_info_t> &&fd_list);
    int remove(const int fd);
    int remove(const std::vector<int> &fd_list);
    int remove(std::vector<int> &&fd_list);

private:
    // TODO: Can this be handled carefull
    // by forward declaring the enum class and
    // defining them in the .cpp file?
    int mplex_to_kqueue(io_mplex_flags_t mplex_values); 
};
