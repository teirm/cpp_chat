// Test cases for BroadCaster class
//
// 26 June 2021

#define CATCH_CONFIG_MAIN

#include "../BroadCaster.hpp"

#include <common/utilities.hpp>

#include <catch2/catch_all.hpp>

#include <memory>
#include <string>
#include <vector>

#include <stdlib.h>


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
/*
TEST_CASE("broadcaster broadcast message", "[broadcast-msg]") {
    int client_count = 4;
    auto broad_caster = BroadCaster();
    char file_template[] = "/tmp/test_client.XXXX";

    std::vector<int> test_clients;
    test_clients.reserve(client_count);
    for (int i = 0; i < client_count; i++) {
        int test_client = mkstemp(file_template);
        REQUIRE(test_client != -1);

        std::string client_name = "client_" + std::to_string(i); 
        test_clients.push_back(test_client);
        broad_caster.add_client(client_name.c_str(), test_client);
    }
    
    int sending_client = test_clients.at(client_count - 1);
    msg_header_t header{4, 10, nullptr};
    broad_caster.broadcast_msg(sending_client, {header, "moo"});
}*/
