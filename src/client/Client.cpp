// Client.cpp
//
// Implementation of client class to communicate with
// cpp chat server
//
// 22 August 2021
//

#include "Client.hpp"

#include <common/log_util.hpp>
#include <common/net_common.hpp>
#include <io_multiplexor/IoMultiplexorFactory.hpp>

#include <exception>
#include <thread>

#include <unistd.h>

constexpr auto MAX_CLIENT_CONNECTIONS = 2;

Client::Client(std::string address, std::string port):
    address_{address},
    port_{port},
    client_socket_{0},
    is_connected_{false} 
{
    
    // The client only has two descriptors it waits on:
    //      stop channel -- for shutting down
    //      client socket -- for receiving messages
    io_mplex_ = IoMultiplexorFactory::get_multiplexor(MAX_CLIENT_CONNECTIONS);
    if (io_mplex_ == nullptr) {
        throw std::runtime_error("unable to allocate multiplexor\n");
    }

    int rc = io_mplex_->add({0, MPLEX_IN, stop_channel_.get_read_end()});
    if (rc != 0) {
        throw std::runtime_error("unable to setup stop channel\n");
    }
}

Client::~Client()
{
    auto rc = disconnect();
    if (rc != 0) {
        log(LogPriority::ERROR, "Failed to stop client -- aborting\n");
        std::abort();
    }
}

// Connect to server specified by address and port
//
// @param[in]   address     address of server
// @param[in]   port        port of server
//
// @return 0 on success
//         non-zero on error
auto Client::connect(const std::string &address, const std::string &port) -> int
{
     
    auto sock_fd = connect_socket(address.c_str(), port.c_str(), false);
    if (sock_fd == -1) {
        return sock_fd;
    }
    client_socket_ = sock_fd;
    is_connected_ = true;
    
    int rc = io_mplex_->add({0, MPLEX_IN, client_socket_});
    if (rc != 0) {
        return rc;
    }
    
    handler_ = std::thread(&Client::receive_messages, std::ref(*this));
    return 0;
}

auto Client::disconnect() -> int 
{
    if (is_connected_) {
        log(LogPriority::INFO, "shutting down client\n");
        auto rc = terminate_connection(client_socket_, SHUT_WR);
        if (rc != 0) {
            log(LogPriority::ERROR, "failed to terminate client connection\n");
            return rc;
        }
        
        // the receiving thread must be running / joinable 
        // for there to be something to stop
        if (handler_.joinable()) {
            if (stop_channel_.write("0") != 1) {
                rc = -1;
                log(LogPriority::ERROR, "failed to stop server\n");
                return rc;
            }
            handler_.join();
        }
    }
    return 0;
}

auto Client::send_message() -> int
{
    return -1;
}

auto Client::get_members() -> int
{
    return -1;
}

auto Client::help() -> int
{
    return -1;
}

auto Client::receive_messages() -> void
{
}


