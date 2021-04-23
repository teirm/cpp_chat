// Server.cpp
//
// Implementation of Server class for 
// handling server state
//
// 22-April-2021

#include "Server.hpp"
#include "Acceptor.hpp"

#include <common/log_util.hpp>
#include <common/net_common.hpp>
#include <common/utilities.hpp>

#include <exception>

#include <sys/epoll.h>

Server::Server(std::string address, std::string port, unsigned int max_conn):
        address_(address),
        port_(port),
        max_conn_(max_conn),
        epoll_fd_(0), 
        is_running_(false)
{

    int server_socket = bind_socket(address_.c_str(), port_.c_str(), false);
    if (server_socket) {
        throw std::runtime_error("Unable to bind socket\n");
    }
    
    int rc = listen_socket(server_socket, 20);
    if (rc) {
        throw std::runtime_error("Unable to mark socket for listening\n");
    }
    
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1) {
        throw std::runtime_error("Unable to create server epoll instance\n");
    }

    struct epoll_event ev;
    ev.data.fd = stop_channel_.read_pipe;
    ev.events = EPOLLIN;
    rc = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, stop_channel_.read_pipe, &ev);
    if (rc == -1) {
        throw std::runtime_error("Unable to add server stop channel to epoll instance\n");
    }

    acceptor_.setSocket(server_socket);
    acceptor_.setServerEpoll(epoll_fd_);
}



int Server::start()
{
    
    
    handle_clients();



    return 0;
}

int Server::stop()
{
    return -1; 

}


void Server::handle_clients()
{
        




}
