// log_util.hpp 
//
// @brief Logging utility functions.
//
// 10 April 2021

#pragma once

#include <type_traits>

#include <sstream>
#include <syslog.h>

// By default the syslog priorities are just a number
// this enum provides some level of type checking

enum class LogPriority: int {
    EMERGENCY   = LOG_EMERG,
    ALERT       = LOG_ALERT,
    CRITICAL    = LOG_CRIT,
    ERROR       = LOG_ERR,
    WARNING     = LOG_WARNING,
    NOTICE      = LOG_NOTICE,
    INFO        = LOG_INFO,
    DEBUG       = LOG_DEBUG
};

#define log(_priority_, _fmt_, ...)         \
    do {                                    \
        static_assert(std::is_same<         \
                        std::decay<         \
                            decltype((_priority_)) \
                        >::type, LogPriority>::value, \
                    "priority is not a LogPriority");\
        syslog(static_cast<int>((_priority_)), (_fmt_), ##__VA_ARGS__); \
    } while (0)
