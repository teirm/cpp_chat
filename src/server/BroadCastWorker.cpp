// BroadCastWorker.cpp
//
// Worker class for handling clients 
// and broadcasting messages.
//
// 16 May 2021

#include "BroadCaster.hpp"

#include <common/log_util.hpp>
#include <thread>


BroadCastWorker::BroadCastWorker()
    :processing_(true)
{
    process_ = std::thread(&BroadCastWorker::process_events, std::ref(*this));
}

BroadCastWorker::~BroadCastWorker()
{
    queue_lock_.lock();
    processing_ = false;
    queue_condition_.notify_one();
    queue_lock_.unlock();
    process_.join();
}

//// 
// @brief add an event to the BroadCastWorker

void BroadCastWorker::add_event(event_info_t &&event_info) 
{
    std::lock_guard<std::mutex> gl(queue_lock_);
    event_queue_.push(event_info);
    queue_condition_.notify_one();
}

////
//  @brief process events enqueued by Server
//
void BroadCastWorker::process_events()
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
            case EventType::DEL_CLIENT:
            case EventType::BROADCAST:
            case EventType::DIRECT_MSG:
            default:
                log(LogPriority::ERROR, "Unknown event type %d\n", event.type);
                std::abort();
        }
    }
}
