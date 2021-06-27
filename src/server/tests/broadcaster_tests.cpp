// Test cases for BroadCaster class
//
// 26 June 2021

#define CATCH_CONFIG_MAIN

#include "../BroadCaster.hpp"

#include <common/utilities.hpp>
#include <common/net_common.hpp>

#include <catch2/catch_all.hpp>

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <thread>
#include <chrono>

const static int TEST_FD = 10;

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
    
    using client_container_t = std::vector<std::pair<std::string, Channel>>;
    using namespace std::chrono_literals;
    
    auto broad_caster = BroadCaster();
    client_container_t test_clients(4);
    
    // add all the clients to the broadcaster 
    int index = 0;
    for (auto &test_client : test_clients) {
        test_client.first = "client" + std::to_string(index);
        index++;
        broad_caster.add_client(test_client.first.c_str(), test_client.second.write_pipe);
    }
    
    // broadcast a message from a single client
    auto &sending_client = test_clients.at(2);
    
    message_t message{{4, 2, nullptr}, "moo"};
    broad_caster.broadcast_msg(sending_client.second.write_pipe, std::move(message));
    
    std::this_thread::sleep_for(2000ms);
}
