// Acceptor.hpp
//
// Class to handle accepting of connections.
// 
// 20 April 2021

#pragma once

#include <common/utilities.hpp>

#include <string>
#include <thread>
#include <atomic>

#include <sys/socket.h>

class Acceptor final {
public:
    Acceptor(int socket_fd, int server_epoll_fd):
        socket_(socket_fd),
        server_epoll_fd_(server_epoll_fd),
        epoll_fd_(0),
        is_running_(false) { }
    Acceptor():Acceptor(0, 0) { }
    ~Acceptor() { }
    
    void setSocket(int socket_fd) { socket_ = socket_fd; }
    void setServerEpoll(int server_epoll_fd) { server_epoll_fd_ = server_epoll_fd; }
    
    int start();
    int stop();

private:
    int socket_;
    int server_epoll_fd_;
    int epoll_fd_;
    Channel stop_channel; 
    std::atomic<bool> is_running_;
    std::thread acceptor_;

    void accept_connections();
    int handle_new_connection();

};
