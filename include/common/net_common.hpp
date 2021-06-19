// net_common.hpp
//
// Header file for common networking code 
// shared between client and server
//
// 10 April 2021

#pragma once

#include <sys/socket.h>

#include <utility>
#include <string>

int bind_socket(const char *address, const char *port, bool is_blocking);

int listen_socket(int socket_fd, int backlog);

int connect_socket(const char *address, const char *port, bool is_blocking);

int sock_writen(int sock_fd, const void *buffer, size_t n_bytes);

int sock_readn(int sock_fd, void *buffer, size_t n_bytes);

std::pair<std::string, bool> get_hostname(struct sockaddr *addr, socklen_t addrlen, int flags);
