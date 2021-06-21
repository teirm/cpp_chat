// net_common.hpp
//
// Header file for common networking code 
// shared between client and server
//
// 10 April 2021

#pragma once

#include "protocol.hpp"

#include <sys/socket.h>

#include <utility>
#include <string>

int bind_socket(const char *address, const char *port, bool is_blocking);

int listen_socket(int socket_fd, int backlog);

int connect_socket(const char *address, const char *port, bool is_blocking);

int write_message(int sock_fd, message_t &&msg);

int read_message(int sock_fd, message_t &msg);

std::pair<std::string, bool> get_hostname(struct sockaddr_storage *addr_storage, socklen_t addrlen, int flags);
