// protocol.hpp
//
// @brief Protocol between the client and the server
//
// 18-June-2021
//

#pragma once

constexpr auto MSG_DATA_MAX_SIZE = 400;

struct msg_header {
    unsigned int  msg_len;
    unsigned long time_stamp;
    char*         target;
};

