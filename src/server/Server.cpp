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
#include <thread>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

Server::Server(std::string address, std::string port, unsigned int max_conn):
        address_(address),
        port_(port),
        server_socket_(-1),
        max_conn_(max_conn),
        is_running_(false)
{ 
    server_socket_ = bind_socket(address_.c_str(), port_.c_str(), false);
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
   
    is_running_ = true;
    handler_ = std::thread(&Server::handle_clients, std::ref(*this));
}

Server::~Server()
{
    is_running_ = false;
    const char *stop_char = "0";
    log(LogPriority::INFO, "Shutting down server\n");
    if (write(stop_channel_.write_pipe, &stop_char, 1) != 1) {
        log(LogPriority::ERROR, "Failed to stop server -- aborting\n");
        std::abort(); 
    }
    handler_.join();
}

void Server::handle_clients()
{
    log(LogPriority::INFO, "Now handling clients at %s:%s\n", address_.c_str(), port_.c_str()); 
    std::vector<io_mplex_fd_info_t> events;
    while (is_running_) {
        int n_events = io_mplex_->wait(nullptr, events);
        if (n_events == -1) {
            log(LogPriority::ERROR, "io_mplex wait error: %s\n", strerror(errno));
            continue;
        }
        for (const auto &event : events) {
            if (event.fd == server_socket_) {
                struct sockaddr_storage client_addr;
                socklen_t addrlen = sizeof(client_addr);
                
                memzero(&client_addr, sizeof(client_addr));
                int client_fd = accept(event.fd, (struct sockaddr *)&client_addr, &addrlen); 
                if (client_fd == -1) {
                    log(LogPriority::ERROR, "accept error: %s\n", strerror(errno));
                    continue;
                }
                auto hostinfo = get_hostname(&client_addr, sizeof(client_addr), NI_NUMERICHOST);
                if (hostinfo.second == false) {
                    log(LogPriority::ERROR, "get_hostname error\n");
                    int err_rc = terminate_connection(client_fd, SHUT_WR);
                    if (err_rc) {
                        log(LogPriority::ERROR, "failed to terminate socket\n");
                    }
                    continue;
                }
                log(LogPriority::INFO, "received connection from %s\n", hostinfo.first.c_str());
                broadcaster_.add_client(hostinfo.first.c_str(), client_fd);
                
                int rc = io_mplex_->add({0, MPLEX_IN | MPLEX_EOF, client_fd});
                if (rc) {
                    log(LogPriority::ERROR, "unable to add client (%s) to multiplexor", hostinfo.first.c_str());
                    int err_rc = terminate_connection(client_fd, SHUT_WR);
                    if (err_rc) {
                        log(LogPriority::ERROR, "failed to terminate socket\n");
                    }
                } 
            } else if (event.fd == stop_channel_.read_pipe) {
                log(LogPriority::INFO, "received shutdown\n");
                break;
            } else {
                if (event.filters & MPLEX_IN) {
                                        
                } else if (event.filters & (MPLEX_EOF | MPLEX_ERR)) {
                    // remove client and terminate connection
                    broadcaster_.del_client(event.fd);
                }
            }
        }
        events.clear();
    }
}
