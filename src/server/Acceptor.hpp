// Acceptor.hpp
//
// Class to handle accepting of connections.
// 
// 20 April 2021


#include <string>

#include <sys/socket.h>

class Acceptor final {
public:
    Acceptor(int socket_fd):
        socket_(socket_fd),
        epoll_fd_(0),
        is_running_(false) { }
    Acceptor(const Acceptor &rhs) = delete;
    Acceptor& operator=(const Acceptor &rhs) = delete;
    ~Acceptor();
    
    int operator()();

    // TODO: stop() should manage the pipe internally
    //       to break out of the select loop. Error of the
    //       acceptor loop should be handled differently...
    //       perhaps a future?
    //       Coordinating shutdown / errors is tricky.
    int stop();

private:
    int socket_;
    int epoll_fd_;
    bool is_running_;
    void accept_connections();
};
