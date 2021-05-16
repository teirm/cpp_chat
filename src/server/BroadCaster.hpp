// Server.hpp
//
// BroadCaster classes for handling client
// info and directing messages to clients.
//
// 16-May-2021

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

enum class EventType : int {
    ADD_CLIENT,
    DEL_CLIENT,
    BROADCAST,
    DIRECT_MSG,
};

struct client_info_t {
    std::string name;
    std::string address;
};

struct event_info_t {
    EventType type;
    int source_fd;
    int dest_fd;
    std::string message;
    std::unique_ptr<client_info_t> client_info;
};
    
class BroadCastWorker final {
public:
    BroadCastWorker();
    ~BroadCastWorker();
    void add_event(event_info_t &&event_info);
private:
    void process_events();
    
    std::mutex queue_lock_;
    std::vector<event_info_t> event_queue_;
    std::unordered_map<int, client_info_t> client_map;
}; 


class BroadCaster final {
public:
    BroadCaster(unsigned int workers);
    ~BroadCaster() {}
    BroadCaster(const BroadCaster &rhs) = delete;
    BroadCaster(BroadCaster &&rhs) = delete;
    BroadCaster& operator()(const BroadCaster &rhs) = delete;
    
    int stop();
    int add_event(event_info_t &&event_info);

private:
    std::vector<BroadCastWorker> workers_;
};
