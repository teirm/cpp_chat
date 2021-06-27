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

#include <utility>

// Write n bytes to a socket in a reliable manner
//
// @param[in]   sock_fd     file descriptor for socket to write to
// @param[in]   buffer      buffer containing data to write
// @param[in]   n_bytes     bytes to write 
//
// @return  total bytes written 
static int sock_writen(int sock_fd, const void *buffer, size_t n_bytes)
{
    int bytes_written = 0;
    int total_written = 0;
    const char *buf = static_cast<const char *>(buffer);

    while ((bytes_written = write(sock_fd, buf, n_bytes))) {
        if (bytes_written <= 0) {
            if (bytes_written == -1 && errno == EINTR) {
                // on signal interupts we restart the write syscal
                continue;
            } else {
                // an actual error occured
                log(LogPriority::ERROR, "error writing to socket, %d\n", errno);
                return total_written;
            }
        }
        buf += bytes_written;
        total_written += bytes_written;
        n_bytes -= bytes_written;
        if (n_bytes == 0) {
            break;
        }
    }
    return total_written;
}

// Read n bytes from a socket in a reliable manner
//
// @param[in] sock_fd   file descriptor for socket to read from
// @parma[in] buffer    buffer to read data into
// @param[in] n_bytes   bytes to read
// 
// @return total bytes read 
static int sock_readn(int sock_fd, void *buffer, size_t n_bytes)
{   
    int bytes_read = 0;
    int total_read = 0;
    char *buf = static_cast<char *>(buffer);
    
    while ((bytes_read = read(sock_fd, buf, n_bytes))) {
        if (bytes_read <= 0) {
            if (bytes_read == -1 && errno == EINTR) {
                // restart syscall if it is interrupted
                continue;
            } else {
                // an actuall error occurred
                log(LogPriority::ERROR, "error reading from socket, %d\n", errno);
                return total_read;
            }
        }
        buf += bytes_read;
        total_read += bytes_read;
        n_bytes -= bytes_read;
        if (n_bytes == 0) {
            break;
        }
    }
    return total_read;
}

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

// Write a message to a socket
//
// @param[in] sock_fd   socket to write to
// @param[in] msg       msg and header to write to socket
//
// @return   0 on success
//          -1 on error
int write_message(int sock_fd, message_t &msg)
{
    size_t write_len = sizeof(msg.header) + msg.header.msg_len; 
    size_t bytes_written = sock_writen(sock_fd, &msg, write_len);
    if (bytes_written < write_len) {
        log(LogPriority::ERROR, "failure to write message: wrote: %lu, expected: %lu",
                bytes_written, write_len);
        return -1;
    }
    return 0;
}

// Read a message form a socket
//
// @param[in]   sock_fd   socket to read form 
// @param[out]  msg       message read from socket
//
// @return      EOF       end of file received on socket
//              0         successful read
//             -1         error  
int read_message(int sock_fd, message_t &msg)
{
    memzero(&msg, sizeof(msg));

    size_t bytes_read = sock_readn(sock_fd, &(msg.header), sizeof(msg.header));
    if (bytes_read == 0) {
        return EOF;
    } else if (bytes_read < sizeof(msg.header)) {
        return -1;
    } else {
        bytes_read = sock_readn(sock_fd, &msg.message, msg.header.msg_len);
        if (bytes_read < msg.header.msg_len) {
            return -1;
        }
    }
    return 0; 
}

// Get the hostname corresponding to an addr structure
//
// @param[in] addr      sockaddr structure to examine
// @param[in] addrlen   size of sockaddr structure
// @param[in] flags     flags for name lookup:
//                        * NI_DGRAM
//                        * NI_NAMEREQD
//                        * NI_NOFDN
//                        * NI_NINUMERICHOST
// 
// @return std::pair<std::string, bool>   
std::pair<std::string, bool> get_hostname(struct sockaddr_storage *addr_storage, socklen_t addrlen, int flags)
{
    char host[NI_MAXHOST];
    memzero(host, sizeof(host));
    struct sockaddr *addr = reinterpret_cast<struct sockaddr*>(addr_storage);

    int rc = getnameinfo(addr, addrlen, host, NI_MAXHOST, nullptr, 0, flags);
    if (rc) {
        log(LogPriority::ERROR, "error getting host name\n");
        return {"", false};
    }

    return {host, true};
}


// Shutdown and close a socket
//
// @param[in]   socket_fd   socket file descriptor to shutdown and close
// @param[in]   flags       shutdown flags
//
// @return  0 on success
//         -1 on error
int terminate_connection(int socket_fd, int flags)
{
    int shut_rc = shutdown(socket_fd, flags);
    if (shut_rc) {
        log(LogPriority::ERROR, "failed to shutdown socket\n");
    }
    int close_rc = close(socket_fd);
    if (close_rc) {
        log(LogPriority::ERROR, "failed to close socket\n");
    }
    
    return (close_rc || shut_rc) ? -1 : 0;
}
