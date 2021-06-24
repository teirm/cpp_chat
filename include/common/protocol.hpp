// protocol.hpp
//
// @brief Protocol between the client and the server
//
// 18-June-2021
//

#pragma once

#include <string>

constexpr auto MSG_DATA_MAX_SIZE = 400;

struct msg_header_t {
    unsigned int   msg_len;
    unsigned long  time_stamp;
    const char    *target;
};

struct message_t {
    msg_header_t header;
    char message[MSG_DATA_MAX_SIZE];
};
