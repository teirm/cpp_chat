// Acceptor.hpp
//
// Class to handle accepting of connections.
// 
// 20 April 2021


#include <string>
#include <thread>
#include <future>
#include <atomic>

#include <sys/socket.h>

class Acceptor final {
public:
    Acceptor(int socket_fd):
        socket_(socket_fd),
        epoll_fd_(0),
        read_pipe_(0),
        write_pipe_(0),
        is_running_(false) { }
    Acceptor(const Acceptor &rhs) = delete;
    Acceptor& operator=(const Acceptor &rhs) = delete;
    ~Acceptor();
    
    int start();
    int stop();

private:
    int socket_;
    int epoll_fd_;
    int read_pipe_;
    int write_pipe_;
    std::atomic<bool> is_running_;
    std::thread acceptor_;

    void accept_connections();
    int handle_new_connection();

};
