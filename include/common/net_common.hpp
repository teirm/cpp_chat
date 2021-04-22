// net_common.hpp
//
// Header file for common networking code 
// shared between client and server
//
// 10 April 2021

#pragma once

int bind_socket(const char *address, const char *port, bool is_blocking);

int listen_socket(int socket_fd, int backlog);

int connect_socket(const char *address, const char *port, bool is_blocking);
