// server_main.cpp
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

#include <thread>
#include <map>
#include <string>

using namespace parse_flags;

const int serverBacklog = 20;

// server main 
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
    
    int server_socket = bind_socket(address.c_str(), port.c_str(), false);
    if (server_socket) {
        log(LogPriority::ERROR, "Unable to bind socket %d\n", server_socket);
        exit(EXIT_FAILURE);
    }
    
    rc = listen_socket(server_socket, serverBacklog);
    if (rc) {
        log(LogPriority::ERROR, "Unable to mark socket for listening: %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
    Acceptor acceptor(server_socket);
    std::thread acceptor_thread(std::ref(acceptor));

    // do server things here

    acceptor_thread.join();
}
