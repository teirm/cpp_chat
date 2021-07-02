// main.cpp
//
// Server main for argument parsing and thread
// initialization. Handles connections and 
// responses.
//
// 20 April 2021

#include "Server.hpp"

#include <common/net_common.hpp>
#include <common/utilities.hpp>
#include <common/log_util.hpp>

#include <cpp/parse_flags.hpp>

#include <string>
#include <csignal>
#include <cstdlib>
#include <thread>

#include <unistd.h>

using namespace parse_flags;

static void sigint_handler(int sig) {
    (void)sig;
}

// @brief Server main argument processing and thread creation 
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
    
    // ignore SIGPIPE to allow for possible EPIPE on writes to 
    // closed/shutdown sockets
    signal(SIGPIPE, SIG_IGN);
    
    // handle SIGINT so that clean shutdown is possible
    signal(SIGINT, sigint_handler); 
   
    Server server(address, port, 20);

    rc = pause();
    if (rc && errno != EINTR) {
        log(LogPriority::ERROR, "failed to pause main thread: %s. Exiting\n", strerror(errno));
    }
}
