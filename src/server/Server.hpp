// Server.hpp
//
// Server class for managing all server state
// and connections.
//
// 22-April-2021

#include "Acceptor.hpp"

#include <string>
#include <common/utilities.hpp>


class Server final {
public:
    Server(std::string address, std::string port, unsigned int max_conn);
    ~Server() { }
    Server(const Server &rhs) = delete;
    Server(Server &&rhs) = delete;
    Server& operator()(const Server &rhs) = delete;
    int start();
    int stop();
private:
    void handle_clients();
    std::string address_;
    std::string port_;
    unsigned int max_conn_;
    int epoll_fd_;
    bool is_running_;
    Channel stop_channel_;
    Acceptor acceptor_;
};
