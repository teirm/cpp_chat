// net_common.cpp
//
// Common networking code shared between
// client and server
//
// 10 April 2021

#include <common/net_common.hpp>
#include <common/utilities.hpp>
#include <common/log_util.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


// Attempt passive open and bind a socket
// at address,port
//
// @param[in] port      port to bind
//
// @return listening socket on success
//         nonzero error code on error
//
// @note intended for server use
int bind_socket(const char *address, const char *port, bool is_blocking)
{
    struct addrinfo hints;
    struct addrinfo *result = nullptr;
    struct addrinfo *rp = nullptr;

    int socket_fd = 0;
    int rc = 0;
    
    memzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;                            /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;                        /* Stream Sockets */
    hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;           /* For wild card IP address */
    hints.ai_protocol = 0;                                  /* Any protocol */
    hints.ai_canonname =  nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;
    
    rc = getaddrinfo(address, port, &hints, &result);
    if (rc) {
        log(LogPriority::ERROR, "getaddrinfo error: %s\n", gai_strerror(rc));
        return rc;
    }

    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        // set O_NONBLOCK 
        auto sock_type = is_blocking ? rp->ai_socktype : rp->ai_socktype|SOCK_NONBLOCK;
        socket_fd = socket(rp->ai_family, sock_type, rp->ai_protocol);
        if (socket_fd == 1) {
            continue; 
        }
        rc = bind(socket_fd, rp->ai_addr, rp->ai_addrlen);
        if (rc == 0) {
            // successfully bound socket
            break;
        }
        close(socket_fd);
    }

    freeaddrinfo(result);
    
    if (rp == nullptr) {
        log(LogPriority::ERROR, "unable to bind any results for %s:%s\n", address, port);
        return 1;
    }
    return socket_fd;
}

// Mark a socket for listening
//
// @param[in] socket_fd     socket to mark for listening
// @param[in] backlog       hint for number of outstanding connections
//
// @return  0 on success
//         1 on error
int listen_socket(int socket_fd, int backlog)
{
    int rc = listen(socket_fd, backlog);
    if (rc != 0) {
        log(LogPriority::ERROR, "listen error: %s\n", strerror(errno));
    }
    return rc;
}

// Attempt to connect to addres,port
//
// @param[in]   address         address to attempt connection
// @param[in]   port            port to attempt connection
// @param[in]   is_blocking     is the socket blocking
//
// @return connected socket on success
//         nonzero error code on error
//
// @note intended for client use
int connect_socket(const char *address, const char *port, bool is_blocking)
{
    struct addrinfo hints;
    struct addrinfo *result = nullptr;
    struct addrinfo *rp     = nullptr;

    int socket_fd = 0;
    int rc = 0;

    memzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_protocol = 0;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    rc = getaddrinfo(address, port, &hints, &result);
    if (rc) { 
        log(LogPriority::ERROR, "getaddrinfo error: %s\n", gai_strerror(rc));
        return rc;
    }

    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        // set O_NONBLOCK 
        auto sock_type = is_blocking ? rp->ai_socktype : rp->ai_socktype|SOCK_NONBLOCK;
        socket_fd = socket(rp->ai_family, sock_type, rp->ai_protocol);
        if (socket_fd == 1) {
            continue;
        }
        rc = connect(socket_fd, rp->ai_addr, rp->ai_addrlen);
        if (rc == 0) {
            break;;
        }
        close(socket_fd);
    }
    
    freeaddrinfo(result);

    if (rp == nullptr) {
        // check for error condition where no connections could be 
        // made
        log(LogPriority::ERROR, "unable to connect any results for %s:%s\n", address, port);
        return 1;
    }
    return socket_fd;
}
