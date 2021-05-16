// Server.hpp
//
// Server class for managing all server state
// and connections.
//
// 22-April-2021

#include <string>
#include <memory>
#include <common/utilities.hpp>
#include <io_multiplexor/IoMultiplexor.hpp>

class Server final {
public:
    Server(std::string address, std::string port, unsigned int max_conn);
    ~Server() { }
    Server(const Server &rhs) = delete;
    Server(Server &&rhs) = delete;
    Server& operator()(const Server &rhs) = delete;
    int start();
    int stop();
private:
    void handle_clients();
    std::string address_;
    std::string port_;
    int server_socket_;
    unsigned int max_conn_;
    bool is_running_;
    std::unique_ptr<IoMultiplexor> io_mplex_;
    Channel stop_channel_; 
};
