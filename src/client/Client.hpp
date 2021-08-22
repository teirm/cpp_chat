// Client.hpp
//
// Client class and associated helper functions
//
// 22 August 2021
//

#include <common/utilities.hpp>
#include <io_multiplexor/IoMultiplexor.hpp>

#include <string>
#include <thread>


class Client final {
public:
    Client(std::string address, std::string port);
    ~Client();
    Client(const Client &rhs) = delete;
    Client(Client &&rhs) = delete;
    Client& operator=(const Client &rhs) = delete;

    int connect(const std::string &address, const std::string &port);
    int disconnect();
    int send_message();
    int get_members();
    int help();

private:
    std::string address_;
    std::string port_;
    std::thread handler_;
    
    int client_socket_;
    bool is_connected_;
    
    std::unique_ptr<IoMultiplexor> io_mplex_;
    Channel stop_channel_;

    void receive_messages();
};
