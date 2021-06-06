// Server.cpp
//
// Implementation of Server class for 
// handling server state
//
// 22-April-2021

#include "Server.hpp"

#include <common/log_util.hpp>
#include <common/net_common.hpp>
#include <common/utilities.hpp>
#include <io_multiplexor/IoMultiplexorFactory.hpp>

#include <exception>

#include <sys/epoll.h>

Server::Server(std::string address, std::string port, unsigned int max_conn):
        address_(address),
        port_(port),
        server_socket_(-1),
        max_conn_(max_conn),
        is_running_(false)
{ server_socket_ = bind_socket(address_.c_str(), port_.c_str(), false);
    if (server_socket_ == -1) {
        throw std::runtime_error("Unable to bind socket\n");
    }
    
    int rc = listen_socket(server_socket_, 20);
    if (rc) {
        throw std::runtime_error("Unable to mark socket for listening\n");
    }
    
    // Two additional entries can be returned -- the listening socket
    // and the stop channel used for shutdown
    io_mplex_ = IoMultiplexorFactory::get_multiplexor(max_conn_ + 2);
    if (io_mplex_ == nullptr) {
        throw std::runtime_error("Unable to allocate multiplexor\n");
    }
    
    rc = io_mplex_->add({0, MPLEX_IN, server_socket_});
    if (rc != 0) {
        throw std::runtime_error("Unable to setup listening socket\n");
    }

    rc = io_mplex_->add({0, MPLEX_IN, stop_channel_.read_pipe});
    if (rc != 0) {
        throw std::runtime_error("Unable to setup pipe\n");
    }
}

int Server::start()
{
    is_running_ = true; 
    handle_clients();

    return 0;
}

int Server::stop()
{
    is_running_ = false;
    return -1; 
}

void Server::handle_clients()
{
    std::vector<io_mplex_fd_info_t> events;
    while (is_running_) {
        int n_events = io_mplex_->wait(nullptr, events);
        if (n_events == -1) {
            log(LogPriority::ERROR, "io_mplex wait error: %s\n", strerror(errno));
            continue;
        }
        for (const auto &event : events) {
            if (event.fd == server_socket_) {
                //TODO: handle accept
            } else if (event.fd == stop_channel_.read_pipe) {
                //TODO: handle shutdown                  
            } else {
                //TODO: everything else should be clients
            }
        }
    }
}
