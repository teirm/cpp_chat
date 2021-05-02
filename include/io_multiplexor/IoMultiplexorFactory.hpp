// IoMultiplexorFactory.hpp
//
// Factory interface for creating iomultiplexors
// based on the platform. 
// 
// 02-May-2021

#include "IoMultiplexor.hpp"
#include "KqueueMultiplexor.hpp"
#include "EpollMultiplexor.hpp"

#include <memory>

class IoMultiplexorFactory final {
    
    IoMultiplexorFactory() = delete;
    ~IoMultiplexorFactory() = delete;

    static std::unique_ptr<IoMultiplexor> get_multiplexor(unsigned max_events);
};


std::unique_ptr<IoMultiplexor> IoMultiplexorFactory::get_multiplexor(unsigned max_events)
{
#if __linux__
    return std::unique_ptr<EpollMultiplexor>(new EpollMultiplexor(max_events));
#elif __freebsd__
    return std::unique_ptr<KqueueMultiplexor>(new KqueueMultiplexor(max_events));
#else
    #error UNSUPPORTED_PLATFORM
#endif
}

