// KqueueMultiplexor.cpp
//
// Implementation of KqueueMultiplexor class.
// Handles IO multiplexing using the Kqueue 
// interface on BSD based systems.
//
//

#include <common/log_util.hpp>
#include <io_multiplexor/KqueueMultiplexor.hpp>

#include <exception>
#include <memory>

#include <unistd.h>
#include <sys/event.h>

using namespace std;

// KqueueMultiplexor constructor
//
// Creates a new kqueue instance
//
// @param[in] max_events    maximum number of supported kevents
//
// @throws std::runtime_error if unable to 
//         create kqueue
KqueueMultiplexor::KqueueMultiplexor(unsigned max_events):
    IoMultiplexor(max_events)
{
    instance_fd_ = kqueue();
    if (instance_fd_ == -1) {
        throw std::runtime_error("Unable to create kqueue");
    }
}

// KqueueMultiplexor destructor
//
// Close the instance of the kqueue
//
KqueueMultiplexor::~KqueueMultiplexor()
{
    if (close(instance_fd_) == -1) {
        log(LogPriority::ERROR, "Failed to close kqueue\n");
    }
}

// Wait for kevents 
//
// @param[in]   timeout     timespec defining the timeout. nullptr implies wait indefinitely
// @param[out]  events      container for events found
//
// @return kevent status 
int KqueueMultiplexor::wait(struct timespec *timeout, std::vector<io_mplex_fd_info_t> &events) 
{
    struct kevent event_list[max_events_]; 

    int count = kevent(instance_fd_, nullptr, 0, event_list, max_events_, timeout);
    if (count == -1 || count == 0) { 
        return count; 
    }
    
    events.reserve(count);
    for (int i = 0; i < count; i++) {
        struct kevent &event = event_list[i]; 
        events.push_back({event.flags, event.filter, event.ident});
    }
    return 0;
}

// Add the given file descriptor with provided flags and filters
// to kqueue.
//
// @param[in]   fd_info     structure defining file descriptor, flags, and filters
//
// @return kevent addition success or failure
int KqueueMultiplexor::add(const io_mplex_fd_info_t &fd_info) 
{
    int flags = mplex_to_kqueue(fd_info.flags);
    int filters = mplex_to_kqueue(fd_info.filters);
    
    struct kevent event;
    EV_SET(&event, fd_info.fd, filters, flags | EV_ADD, 0, 0, nullptr);
    
    int rc =  kevent(instance_fd_, &event, 1, nullptr, 0, nullptr);
    if (rc) {
        log(LogPriority::ERROR, "Failed to add event to kqueue\n");
    } else {
        n_events_++;
    }
    
    return rc;
}

// Add the set of provided file descriptors and associated flags/filters
// to kqueue.
//
// @param[in]   fd_info_list    list of file descriptors, each with flags and filters
//
// @return kevent addition success or failutre
//
// @note addition is all or nothing
int KqueueMultiplexor::add(const std::vector<io_mplex_fd_info_t> &fd_list)
{
    size_t size = fd_list.size();
    auto change_list = unique_ptr<struct kevent []>(new struct kevent[size]);
    int index = 0;
    int flags = 0;
    int filters = 0;
    for (auto &fd_info : fd_list) {
        flags = mplex_to_kqueue(fd_info.flags);
        filters = mplex_to_kqueue(fd_info.filters);
        EV_SET(&change_list[index], fd_info.fd, filters, flags | EV_ADD, 0, 0, nullptr);
        index++;
    }

    int rc = kevent(instance_fd_, change_list.get(), size, nullptr, 0, nullptr);
    if (rc) {
        log(LogPriority::ERROR, "Failed to add event list to kqueue\n");
    } else {
        n_events_.fetch_add(size);
    }

    return rc;
}

// Add the set of provided file descriptors and associated flags/filters
// to kqueue.
//
// @param[in]   fd_info_list    list of file descriptors, each with flags and filters
//
// @return kevent addition success or failutre
//
// @note addition is all or nothing
int KqueueMultiplexor::add(std::vector<io_mplex_fd_info_t> &&fd_list)
{
    return add(fd_list);
}

// Remove the file descriptor from the kqueue.
//
// @param[in]   fd  file descriptor to remove 
//
// @return kevent success or failure
int KqueueMultiplexor::remove(const int fd) 
{
    struct kevent event;
    // add the EVFILT_READ filter incase there is anything remaining to be 
    EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    
    int rc = kevent(instance_fd_, &event, 1, nullptr, 0, nullptr);
    if (rc) {
        log(LogPriority::ERROR, "Failed to delete fd from kqueue\n");
    } else {
        n_events_--;
    }

    return rc;
}

// Remove the set of file descriptors from the kqueue
//
// @param[in]   fd file descriptor to remove
//
// @return kevent success or failure
//
// @note removal is all or nothing
int KqueueMultiplexor::remove(const std::vector<int> &fd_list) 
{
    size_t size = fd_list.size();
    auto change_list = unique_ptr<struct kevent []>(new struct kevent[size]);
    int index = 0;
    for (auto &fd : fd_list) {
        EV_SET(&change_list[index], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
        index++;
    }
    
    int rc = kevent(instance_fd_, change_list.get(), size, nullptr, 0, nullptr);
    if (rc ) {
        log(LogPriority::ERROR, "Failed to remove events from kqueue\n");
    } else {
        n_events_.fetch_sub(size);
    }
    
    return rc;
}

// Remove the set of file descriptors from the kqueue
//
// @param[in]   fd file descriptor to remove
//
// @return kevent success or failure
//
// @note removal is all or nothing
int KqueueMultiplexor::remove(std::vector<int> &&fd_list)
{
    return remove(fd_list);
}

// Translate platform agnostic multiplexor flags to kqueue
// specific flags and filters
// 
// @param[in]   mplex_values        multiplexor values (flags or filters)
//
// @return kqueue specific flags or filters
int KqueueMultiplexor::mplex_to_kqueue(io_mplex_flags_t mplex_values)
{
    int flags = 0;

    if (mplex_values & MPLEX_IN) {
        flags |= EVFILT_READ;
    } 
    if (mplex_values & MPLEX_OUT) {
        flags |= EVFILT_WRITE;
    }
    if (mplex_values & MPLEX_ONESHOT) {
        flags |= EV_ONESHOT;
    }
    if (mplex_values & MPLEX_EOF) {
        flags |= EV_EOF;
    }
    return flags;
}
