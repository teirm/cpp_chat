// Server.hpp
//
// Server class for managing all server state
// and connections.
//
// 22-April-2021

#include "BroadCaster.hpp"

#include <common/utilities.hpp>
#include <io_multiplexor/IoMultiplexor.hpp>

#include <string>
#include <memory>
#include <thread>
#include <atomic>

class Server final {
public:
    Server(std::string address, std::string port, unsigned int max_conn);
    ~Server();
    Server(const Server &rhs) = delete;
    Server(Server &&rhs) = delete;
    Server& operator()(const Server &rhs) = delete;

private:
    std::string address_;
    std::string port_;
    std::thread handler_;

    int server_socket_;
    unsigned int max_conn_;
    std::atomic<bool> is_running_;
    
    std::unique_ptr<IoMultiplexor> io_mplex_;
    BroadCaster broadcaster_;
    Channel stop_channel_;

    void handle_clients();
};
