// BroadCaster.hpp
//
// BroadCaster classes for handling client
// info and directing messages to clients.
//
// 16-May-2021

#include <common/protocol.hpp>

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <unordered_map>

#include <cassert>

enum class EventType : int {
    ADD_CLIENT,
    DEL_CLIENT,
    BROADCAST,
    DIRECT_MSG,
};

struct event_info_t {
    EventType type;
    int sock_fd;
    const char *source;
    message_t message;
};

class BroadCaster final {
public:
    BroadCaster();
    ~BroadCaster();
    
    BroadCaster(const BroadCaster &rhs) = delete;
    BroadCaster(BroadCaster &&rhs) = delete;
    BroadCaster& operator()(const BroadCaster &rhs) = delete;
   
    ////
    // @brief add a client to the BroadCaster
    // 
    // @param[in]   name        name of the client
    // @param[in]   client_fd   connection to client
    //
    void add_client(const char *name, int client_fd)
    {
        assert(name != nullptr);
        add_event({EventType::ADD_CLIENT, client_fd, name, {}});
    }

    ////
    // @brief delete a client from the BroadCaster
    //
    // @param[in]   client_fd       client file descriptor to delete 
    void del_client(int client_fd)
    {
        add_event({EventType::DEL_CLIENT, client_fd, nullptr, {}});
    }
    
    ////
    // @brief broadcast a message
    //
    // @param[in]  source   source of the message
    // @param[in]  message  message to broadcast
    void broadcast_msg(int client_fd, message_t &&message)
    {
        add_event({EventType::BROADCAST, client_fd, nullptr, message});
    }

    ////
    // @brief direct message 
    //
    // @param[in]   source  source of the message
    // @param[in]   intended recipient of the message
    // @param[in]   message to send
    void direct_msg(int client_fd, message_t &&message) {
        add_event({EventType::DIRECT_MSG, client_fd, nullptr, message});
    }
    
    void read_message(int client_fd);

private:

    void process_events();

    void add_event(event_info_t &&event_info);
    bool processing_;
    std::thread process_;
    std::condition_variable queue_condition_;
    std::mutex queue_lock_;
    std::queue<event_info_t> event_queue_;
    std::unordered_map<int, const char*> client_map_;
};
