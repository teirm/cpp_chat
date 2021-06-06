// BroadCastWorker.cpp
//
// Worker class for handling clients 
// and broadcasting messages.
//
// 16 May 2021

#include "BroadCaster.hpp"

#include <common/log_util.hpp>
#include <thread>


BroadCaster::BroadCaster()
    :processing_(true)
{
    process_ = std::thread(&BroadCaster::process_events, std::ref(*this));
}

BroadCaster::~BroadCaster()
{
    queue_lock_.lock();
    processing_ = false;
    queue_condition_.notify_one();
    queue_lock_.unlock();
    process_.join();
}

//// 
// @brief add an event to the BroadCastWorker
void BroadCaster::add_event(event_info_t &&event_info) 
{
    std::lock_guard<std::mutex> gl(queue_lock_);
    event_queue_.push(event_info);
    queue_condition_.notify_one();
}

////
//  @brief process events enqueued by Server
//
void BroadCaster::process_events()
{
    while (processing_) {
        std::unique_lock<std::mutex> lk(queue_lock_);
        if (event_queue_.empty()) {
            queue_condition_.wait(lk, [this]{ return !processing_ || !event_queue_.empty(); });
        } 
        event_info_t event = event_queue_.front();
        event_queue_.pop();
        lk.unlock();

        switch (event.type) {
            case EventType::ADD_CLIENT: 
            {
                auto res = client_map_.insert({event.name, event.sock_fd});
                if (res.second == false) {
                    log(LogPriority::ERROR, "Failed to insert client %s\n", 
                            event.name.c_str());
                }
            }
            break;
            case EventType::DEL_CLIENT: 
            {
                auto count = client_map_.erase(event.name);
                if (count == 0) {
                    log(LogPriority::ERROR, "Failed to remove client\n");
                }
            }
            break;
            case EventType::BROADCAST:
            {   
                // Write the message to all known clients except
                // the sending client
                for (auto &client : client_map_) {
                    int dest_fd = client.second;
                    if (dest_fd != event.sock_fd) {
                        int rc = send_message(dest_fd, std::move(event.message));
                        if (rc) {
                            log(LogPriority::ERROR, "Failed to send message to client %s\n",
                                client.first.c_str());
                        }
                    }
                }
            }
            break;
            case EventType::DIRECT_MSG:
            {
                auto res = client_map_.find(event.target);
                if (res == client_map_.end()) {
                    log(LogPriority::INFO, "Destination %s not found\n", event.target.c_str());
                }
                int rc = send_message(res->second, std::move(event.message));
                if (rc) {
                    log(LogPriority::INFO, "Failed to send message to %s\n", event.target.c_str());
                }
            }
                break;
            default:
                log(LogPriority::ERROR, "Unknown event type %d\n", event.type);
                std::abort();
        }
    }
}


int BroadCaster::send_message(int dest_fd, std::string &&message)
{
    (void)dest_fd;
    (void)message;
    //TODO: Need to think about client and server protocol
    return -1;
}
