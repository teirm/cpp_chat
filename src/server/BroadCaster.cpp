// BroadCastWorker.cpp
//
// Worker class for handling clients 
// and broadcasting messages.
//
// 16 May 2021

#include "BroadCaster.hpp"

#include <common/log_util.hpp>
#include <common/net_common.hpp>

#include <cassert>
#include <thread>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

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
// @brief read a message from the client
//
// @param[in]   client_fd   client to read message from
void BroadCaster::read_message(int client_fd)
{
    message_t message;
    int rc = ::read_message(client_fd, message);
    if (rc) {
        log(LogPriority::ERROR, "failed to read message from client\n");
        return;
    }
    if (message.header.target == nullptr) {
        add_event({EventType::BROADCAST, client_fd, nullptr, std::move(message)});
    } else {
        add_event({EventType::DIRECT_MSG, client_fd, nullptr, std::move(message)});
    }
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
        if (processing_ == false) {
            // shutting down -- any events left over are just dropped
            log(LogPriority::INFO, "Shutting down broadcaster -- remaining events: %lu\n", 
                    event_queue_.size());
            lk.unlock();
            break;
        }
        event_info_t event = event_queue_.front();
        event_queue_.pop();
        lk.unlock();

        switch (event.type) {
            case EventType::ADD_CLIENT: 
            {
                auto res = client_map_.insert({event.sock_fd, event.source});
                if (res.second == false) {
                    log(LogPriority::ERROR, "Failed to insert client %s\n", 
                            event.source);
                }
            }
            break;
            case EventType::DEL_CLIENT: 
            {
                auto count = client_map_.erase(event.sock_fd);
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
                    int dest_fd = client.first;
                    if (dest_fd != event.sock_fd) {
                        int rc = write_message(dest_fd, event.message);
                        if (rc) {
                            log(LogPriority::ERROR, "Failed to send message to client %s\n",
                                client.second);
                        }
                    }
                }
            }
            break;
            case EventType::DIRECT_MSG:
            {
                bool found = false;
                const char *target = event.message.header.target;
                for (auto &client : client_map_) {
                    if (strncmp(client.second, target, strlen(target)) == 0) {
                        int rc = write_message(client.first, event.message);
                        if (rc) {
                            log(LogPriority::INFO, "Failed to send message to %s\n", target);
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    log(LogPriority::INFO, "Unable to send message to %s\n", target);
                }
            }
            break;
            default:
                log(LogPriority::ERROR, "Unknown event type %d\n", static_cast<int>(event.type));
                std::abort();
        }
    }
    
    int rc = 0;
    // Shutdown all client connections and close sockets
    for (auto &client_info : client_map_) {
        rc = terminate_connection(client_info.first, SHUT_WR);
        if (rc) {
            log(LogPriority::ERROR, "Failed to terminate connection to %s\n", client_info.second);
        }
    }
}
