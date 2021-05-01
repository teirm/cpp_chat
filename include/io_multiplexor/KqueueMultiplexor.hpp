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
    KqueueMultiplexor(unsigned max_events);
    ~KqueueMultiplexor();
    
    virtual int wait(struct timespec *timeout, std::vector<io_mplex_fd_info_t> &events) override;
    
    virtual int add(const io_mplex_fd_info_t &fd_info) override;
    virtual int add(const std::vector<io_mplex_fd_info_t> &fd_list) override;
    virtual int add(std::vector<io_mplex_fd_info_t> &&fd_list) override;
    
    virtual int remove(const int fd) override;
    virtual int remove(const std::vector<int> &fd_list) override;
    virtual int remove(std::vector<int> &&fd_list) override;

private:
    // TODO: Can this be handled carefull
    // by forward declaring the enum class and
    // defining them in the .cpp file?
    int mplex_to_kqueue(io_mplex_flags_t mplex_values); 
};
