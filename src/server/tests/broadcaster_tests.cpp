// Test cases for BroadCaster class
//
// 26 June 2021

#define CATCH_CONFIG_MAIN

#include "../BroadCaster.hpp"

#include <common/utilities.hpp>
#include <common/net_common.hpp>
#include <io_multiplexor/IoMultiplexorFactory.hpp>

#include <catch2/catch_all.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include <unistd.h>

const static int TEST_FD = 10;
const static unsigned int NUM_CLIENTS = 4;
    
using client_container_t = std::vector<std::pair<std::string, Channel>>;
using event_container_t  = std::vector<io_mplex_fd_info_t>;

TEST_CASE("broadcaster is createable", "[construct]") {
    REQUIRE_NOTHROW(BroadCaster());
}

TEST_CASE("broadcaster add client", "[add-client]") {
    auto broad_caster = BroadCaster();
    broad_caster.add_client("test_client", TEST_FD);    
}

TEST_CASE("broadcaster add/delete client", "[add-del-client]") {
    auto broad_caster = BroadCaster();
    broad_caster.add_client("test_client", TEST_FD);
    broad_caster.del_client(TEST_FD);
}

TEST_CASE("broadcaster broadcast message", "[broadcast-msg]") {
    auto broad_caster = BroadCaster();
    auto io_mplex = IoMultiplexorFactory::get_multiplexor(NUM_CLIENTS);
    client_container_t test_clients(NUM_CLIENTS);
    
    // add all the clients to the broadcaster 
    int index = 0;
    for (auto &test_client : test_clients) {
        test_client.first = "client" + std::to_string(index);
        index++;
        broad_caster.add_client(test_client.first.c_str(), test_client.second.get_write_end());
        REQUIRE(io_mplex->add({0, MPLEX_IN | MPLEX_ONESHOT, test_client.second.get_read_end()}) == 0);
        INFO("Client: " << test_client.first << " fd: " << test_client.second.get_read_end());
    }
    
    // broadcast a message from a single client
    auto &sending_client = test_clients.at(2);
    REQUIRE(io_mplex->remove(sending_client.second.get_read_end()) == 0);

    message_t message{{4, 2, nullptr}, "moo"};
    broad_caster.broadcast_msg(sending_client.second.get_write_end(), std::move(message));

    event_container_t  test_events(NUM_CLIENTS);
    int remaining_responses = NUM_CLIENTS - 1;
    while (remaining_responses > 0) {
        int events = io_mplex->wait(nullptr, test_events);
        INFO("events received: " << events);
        REQUIRE(events != -1);
        for (const auto &event : test_events) {
            if (event.filters & MPLEX_IN) {
                message_t received_msg;
                INFO("Reading from: " << event.fd);
                int rc = read_message(event.fd, received_msg);
                REQUIRE(rc == 0);
                REQUIRE(strncmp(received_msg.message, "moo", strlen("moo")) == 0);
                REQUIRE(close(event.fd) == 0);
            }
        }
        test_events.clear();
        remaining_responses -= events;
        INFO("remaining events: " << remaining_responses);
    }
}

TEST_CASE("broadcaster direct message", "[direct-msg]") {
    Channel sender;
    Channel receiver;
    auto broad_caster = BroadCaster();
    
    broad_caster.add_client("sender", sender.get_write_end());
    broad_caster.add_client("receiver", receiver.get_write_end());   

    auto io_mplex = IoMultiplexorFactory::get_multiplexor(2);
    REQUIRE(io_mplex);
    REQUIRE(io_mplex->add({0, MPLEX_IN | MPLEX_ONESHOT, receiver.get_read_end()}) == 0);

    broad_caster.direct_msg(sender.get_write_end(), {{5, 2, "receiver"}, "test"});
    
    event_container_t test_events;
    int events = io_mplex->wait(nullptr, test_events);
    REQUIRE(events == 1);
    REQUIRE(test_events.size() == 1);
    
    auto event = test_events.front();
    REQUIRE(event.filters & MPLEX_IN);
    
    message_t received_msg;
    REQUIRE(read_message(event.fd, received_msg) == 0);
    REQUIRE(strncmp(received_msg.message, "test", strlen("moo")) == 0);
    REQUIRE(close(event.fd) == 0);
}
