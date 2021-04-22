// Acceptor.cpp
//
// Implementation of the acceptor class.
//
// Uses epoll to manage the accept socket
// and an internal pipe to trigger shutdown.
//
// A future is granted on start to provide
// a way to indicate errors to other threads.
//
// 20 April 2021

#include "Acceptor.hpp"

#include <common/log_util.hpp>
#include <common/utilities.hpp>

#include <cassert>

#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
// TODO: only building on linux right now so just use epoll. 
//       Can add kqueue support later
#include <sys/epoll.h>

////
// @brief Start accepting connections.
//
// @return 0 on success
//        -1 on error
//
int Acceptor::start()
{
    // epoll ignores the size argument but it must be
    // greater than zero.
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1) {
        log(LogPriority::ERROR,  "Unable to create epoll instance: %s\n", strerror(errno));
        return -1;
    }
    
    // add the accepting socket and the read end of the pipe to the epoll instance
    // for monitoring
    struct epoll_event ev;
    ev.data.fd = stop_channel.read_pipe;
    ev.events = EPOLLIN;
    int rc = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, stop_channel.read_pipe, &ev);
    if (rc == -1) {
        log(LogPriority::ERROR, "Unable to add read pipe to epoll instance: %s\n", strerror(errno));
        return -1;
    }
    
    ev.data.fd = socket_;
    ev.events = EPOLLIN;
    rc = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_, &ev);
    if (rc == -1) {
        log(LogPriority::ERROR, "Unable to add socket to epoll instance: %s\n", strerror(errno));
        return -1;
    }
    
    // begin accepting connections
    is_running_ = true;
    // TODO: How to handle exceptions here?
    acceptor_ = std::thread(&Acceptor::accept_connections, this); 

    return 0;
}

// Used to stop accepting connections
//
//
int Acceptor::stop()
{
    is_running_ = false;
    // write a single byte into the pipe to trigger stop
    int n_written = write(stop_channel.write_pipe, "s", 1);
    if (n_written != 1) {
        log(LogPriority::ERROR, "Error writing to stop pipe\n");
        return -1;
    }
    return 0;
}

int Acceptor::handle_new_connection()
{
    socklen_t        addr_len = 0;
    char             host_buff[NI_MAXHOST];
    char             service_buff[NI_MAXSERV];
    sockaddr_storage client_addr;
    
    memzero(&client_addr, sizeof(client_addr));
    memzero(host_buff, sizeof(host_buff));
    memzero(service_buff, sizeof(service_buff));
    
    int cli_socket = accept(socket_, reinterpret_cast<sockaddr *>(&client_addr), &addr_len);
    if (cli_socket) {
        log(LogPriority::ERROR, "accept error\n");
        return -1;
    }

    int rc = getnameinfo(reinterpret_cast<sockaddr *>(&client_addr), addr_len,
                         host_buff, sizeof(host_buff), service_buff, sizeof(service_buff),
                         NI_NUMERICHOST|NI_NUMERICSERV);
    if (rc) {
        log(LogPriority::ERROR, "getnameinfo error: %s\n", strerror(errno));
        return -1;
    }

    // TODO: Need to add cli_socket to various other structures
    //       client handling epoll instance
    //       broadcaster(s)
    return 0;
}


void Acceptor::accept_connections() 
{
    // there should only ever be two possible events...
    // accept and read on pipe
    struct epoll_event evlist[2];

    while (is_running_) {
        // wait indefinitely for an event
        int n_ready = epoll_wait(epoll_fd_, evlist, sizeof(evlist), -1);
        if (n_ready == -1) {
            // TODO: errno is strange in a multithreaded context
            log(LogPriority::ERROR, "epoll error: %s\n", strerror(errno));
            // try again?
            continue;
        }

        for (int i = 0; i < n_ready; i++) {
            if (evlist[i].events & EPOLLIN) {
                if (evlist[i].data.fd == socket_) {
                    handle_new_connection();  
                } else if (evlist[i].data.fd == stop_channel.read_pipe)  {
                    // don't have to do anything since the is_running flag should
                    // be set to false now. 
                    log(LogPriority::INFO, "Stop event seen -- acceptor stopping\n");  
                } else {
                    // this would be a programmer error
                    assert("unknown file descriptor encountered\n");
                }
            } else if (evlist[i].events & (EPOLLERR|EPOLLHUP)) {
                log(LogPriority::ERROR, "events %d seen on %d\n", evlist[i].events, evlist[i].data.fd);
            } else {
                log(LogPriority::INFO, "unknown event %d encountered on %d\n", 
                    evlist[i].events, evlist[i].data.fd);
            }
        }

    }
}
