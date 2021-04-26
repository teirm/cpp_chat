// main.cpp
//
// Server main for argument parsing and thread
// initialization. Handles connections and 
// responses.
//
// 20 April 2021

#include "Acceptor.hpp"

#include <common/net_common.hpp>
#include <common/utilities.hpp>
#include <common/log_util.hpp>

#include <cpp/parse_flags.hpp>

#include <string>

#include <sys/epoll.h>

using namespace parse_flags;

const int serverBacklog = 20;

// @brief Server main argument processing and begins connection handling 
int main(int argc, char *argv[])
{
    std::string port;
    std::string address;

    ParseFlags parser;
    parser.add_flag("port", port, "port for server to use");
    parser.add_flag("address", address, "address for server");
    int rc = parser.parse_args(argc, argv);
    if (rc) {
        log(LogPriority::ERROR, "Unable to parse arguments: %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
     
    

}