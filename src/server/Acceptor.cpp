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

#include <netdb.h>
#include <sys/socket.h>
// TODO: only building on linux right now so just use epoll. 
// Can add kqueue support later
#include <sys/epoll.h>

int Acceptor::operator()()
{
    return -1;
}

int Acceptor::stop()
{
    return -1;
}

void Acceptor::accept_connections() 
{
    int              rc = 0; 
    socklen_t        addr_len = 0;
    char             host_buff[NI_MAXHOST];
    char             service_buff[NI_MAXSERV];
    sockaddr_storage client_addr;

    while (is_running_) {
        memzero(&client_addr, sizeof(client_addr));
        memzero(host_buff, sizeof(host_buff));
        memzero(service_buff, sizeof(service_buff));
        
        rc = accept(socket_, reinterpret_cast<sockaddr *>(&client_addr), &addr_len);
        if (rc) {
            log(LogPriority::ERROR, "accept error: %s\n", strerror(rc));
            break;
        }

        rc = getnameinfo(reinterpret_cast<sockaddr *>(&client_addr), addr_len,
                         host_buff, sizeof(host_buff), service_buff, sizeof(service_buff),
                         NI_NUMERICHOST|NI_NUMERICSERV);
        if (rc) {
            log(LogPriority::ERROR, "getnameinfo error: %s\n", strerror(rc));
            break;
        }
    }
}
