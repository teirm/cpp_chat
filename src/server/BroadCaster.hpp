// BroadCaster.hpp
//
// BroadCaster classes for handling client
// info and directing messages to clients.
//
// 16-May-2021

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <unordered_map>

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
    const char *destination;
    const char *message;
};

class BroadCaster final {
public:
    BroadCaster();
    ~BroadCaster();
    
    BroadCaster(const BroadCaster &rhs) = delete;
    BroadCaster(BroadCaster &&rhs) = delete;
    BroadCaster& operator()(const BroadCaster &rhs) = delete;
   
    void add_client(const char *name, int client_fd);
    void del_client(const char *name);
    void broadcast_msg(const char *source, const char *message);
    void direct_msg(const char *source, const char *destination, const char *message);

private:

    void process_events();

    int send_message(int dest_fd, std::string &&message);

    void add_event(event_info_t &&event_info);
    bool processing_;
    std::thread process_;
    std::condition_variable queue_condition_;
    std::mutex queue_lock_;
    std::queue<event_info_t> event_queue_;
    std::unordered_map<const char *, int> client_map_;
};
