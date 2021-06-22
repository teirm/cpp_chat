// BroadCastWorker.cpp
//
// Worker class for handling clients 
// and broadcasting messages.
//
// 16 May 2021

#include "BroadCaster.hpp"

#include <common/log_util.hpp>

#include <cassert>
#include <thread>

#include <unistd.h>

#include <sys/socket.h>

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
// @brief add a client to the BroadCaster
// 
// @param[in]   name        name of the client
// @param[in]   client_fd   connection to client
//
void BroadCaster::add_client(const char *name, int client_fd)
{
    assert(name != nullptr);
    return add_event({EventType::ADD_CLIENT, client_fd, name, nullptr, nullptr});
}

////
// @brief delete a client from the BroadCaster
//
// @param[in]   name        name of the client to delete
void BroadCaster::del_client(const char *name)
{
    assert(name != nullptr);
    return add_event({EventType::DEL_CLIENT, 0, name, nullptr, nullptr});
}

////
// @brief broadcast a message
//
// @param[in]  source   source of the message
// @param[in]  message  message to broadcast
void BroadCaster::broadcast_msg(const char *source, const char *message)
{
    assert(source != nullptr);
    assert(message != nullptr);
    return add_event({EventType::BROADCAST, 0, source, nullptr, message});
}

////
// @brief direct message 
//
// @param[in]   source  source of the message
// @param[in]   intended recipient of the message
// @param[in]   message to send
void BroadCaster::direct_msg(const char *source, const char *destination, const char *message)
{
    assert(source != nullptr);
    assert(destination != nullptr);
    assert(message != nullptr);
    return add_event({EventType::DIRECT_MSG, 0, source, destination, message});
}

//// 
// @brief add an event to the BroadCaster
//
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
                auto res = client_map_.insert({event.source, event.sock_fd});
                if (res.second == false) {
                    log(LogPriority::ERROR, "Failed to insert client %s\n", 
                            event.source);
                }
            }
            break;
            case EventType::DEL_CLIENT: 
            {
                auto count = client_map_.erase(event.source);
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
                                client.first);
                        }
                    }
                }
            }
            break;
            case EventType::DIRECT_MSG:
            {
                auto res = client_map_.find(event.destination);
                if (res == client_map_.end()) {
                    log(LogPriority::INFO, "Destination %s not found\n", event.destination);
                }
                int rc = send_message(res->second, std::move(event.message));
                if (rc) {
                    log(LogPriority::INFO, "Failed to send message to %s\n", event.destination);
                }
            }
            break;
            default:
                log(LogPriority::ERROR, "Unknown event type %d\n", event.type);
                std::abort();
        }
    }
    
    int rc = 0;
    // Shutdown all client connections and close sockets
    for (auto &client_info : client_map_) {
        rc = shutdown(client_info.second, SHUT_WR);
        if (rc) {
            log(LogPriority::ERROR, "Failed to shutdown connection to %s\n", client_info.first);
        }
        rc = close(client_info.second);
        if (rc) {
            log(LogPriority::ERROR, "Failed to close connection to %s\n", client_info.first);
        }
    }
}

////
// @brief Send a message to the given socket file descriptor
//
// @param[in] sock_fd   socket on which to send message
// @param[in] message   message to write 

int BroadCaster::send_message(int sock_fd, std::string &&message)
{
    (void)sock_fd;
    (void)message;
    //TODO: Need to think about client and server protocol
    return -1;
}
