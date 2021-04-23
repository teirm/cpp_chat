// io_multiplexing.hpp
//
// Common interface to different types of 
// io_multiplexing facilities.
//
// 22-April-2021

#include <stdlib.h>
#include <sys/time.h>

// Create a multiplexer
int io_multiplexer_create();

int io_multiplexer_wait(int io_multi_fd, void *event_list, size_t n_events, const struct timespec *timeout); 

int io_multiplexer_add(int io_multi_fd, int fd); 
