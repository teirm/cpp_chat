// Test cases for io_multiplexor

#define CATCH_CONFIG_MAIN

#include <common/utilities.hpp>
#include <io_multiplexor/IoMultiplexorFactory.hpp>

#include <catch2/catch_all.hpp>

#include <memory>
#include <string>

#include <unistd.h>

const unsigned test_mplex_size = 10;

TEST_CASE("multiplexor is createable", "[construct]") {
    
    REQUIRE_NOTHROW(IoMultiplexorFactory::get_multiplexor(test_mplex_size));
}

TEST_CASE("multiplexor test flags and filters", "[flags_and_filters]") {
    Channel test_channel;
    auto mplex = IoMultiplexorFactory::get_multiplexor(test_mplex_size);
     
    SECTION("add read") {
        REQUIRE(mplex->add({0, MPLEX_IN, test_channel.read_pipe}) == 0);
    }

    SECTION("add write") {
        REQUIRE(mplex->add({0, MPLEX_OUT, test_channel.write_pipe}) == 0);
    }

    SECTION("add oneshot") {
        REQUIRE(mplex->add({MPLEX_ONESHOT, MPLEX_IN, test_channel.read_pipe}) == 0);
    }

    SECTION("add EOF") {
        REQUIRE(mplex->add({MPLEX_EOF, MPLEX_IN, test_channel.read_pipe}) == 0);
    }

    SECTION("add error") {
        REQUIRE(mplex->add({MPLEX_ERR, MPLEX_IN, test_channel.read_pipe}) == 0);
    }
}

TEST_CASE("multiplex add list", "[add_list]") {
    Channel channel_1;
    Channel channel_2;

    auto mplex = IoMultiplexorFactory::get_multiplexor(test_mplex_size);
    
    REQUIRE(mplex->add({{MPLEX_ONESHOT, MPLEX_IN, channel_1.read_pipe},
                        {0, MPLEX_OUT, channel_1.write_pipe},
                        {MPLEX_EOF, MPLEX_IN, channel_2.read_pipe},
                        {0, MPLEX_OUT, channel_2.write_pipe}}) == 0);
}

TEST_CASE("multiplexor timeout", "[timeout]") {
    Channel test_channel;
    struct timespec timeout{3, 0};

    auto mplex = IoMultiplexorFactory::get_multiplexor(test_mplex_size);

    REQUIRE(mplex->add({0, MPLEX_IN, test_channel.read_pipe}) == 0);
    
    std::vector<io_mplex_fd_info_t> events;
    REQUIRE(mplex->wait(&timeout, events) == 0);
    REQUIRE(events.size() == 0); 
}

TEST_CASE("multiplexor returns write", "[write_ready]") {
    Channel test_channel;
    auto mplex = IoMultiplexorFactory::get_multiplexor(test_mplex_size);

    REQUIRE(mplex->add({0, MPLEX_OUT, test_channel.write_pipe}) == 0);
    
    std::vector<io_mplex_fd_info_t> events;
    REQUIRE(mplex->wait(nullptr, events) == 1);
    REQUIRE(events.size() == 1);
    auto& event = events[0];
    REQUIRE(event.fd == test_channel.write_pipe);
    REQUIRE(event.filters == MPLEX_OUT);
}

TEST_CASE("multiplexor returns read", "[read_ready]") {
    Channel test_channel;
    
    // Write something to the pipe for the multiplexor to find
    std::string message{"akkoXdianna"};
    REQUIRE(write(test_channel.write_pipe, message.c_str(), message.size()) 
                == static_cast<ssize_t>(message.size()));
    
    // Add pipe to io mplex
    auto mplex = IoMultiplexorFactory::get_multiplexor(test_mplex_size);
    REQUIRE(mplex->add({0, MPLEX_IN, test_channel.read_pipe}) == 0);

    // Check for event
    std::vector<io_mplex_fd_info_t> events;
    REQUIRE(mplex->wait(nullptr, events) == 1);
    REQUIRE(events.size() == 1);
    auto& event = events[0];
    REQUIRE(event.fd == test_channel.read_pipe);
    REQUIRE(event.filters == MPLEX_IN);

    // Read message from pipe
    char msg_buffer[message.size() + 1];
    REQUIRE(read(test_channel.read_pipe, msg_buffer, sizeof(msg_buffer)) ==
                static_cast<ssize_t>(message.size()));
}
