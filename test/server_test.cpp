#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "server/inc/server.hpp"

using namespace bnetw::networking;

TEST(server_test, test1)
{
    unsigned short port_num = 888;
    server s(port_num);
}

TEST(server_test, test2)
{
    unsigned short port_num = 888;
    server s(port_num);
    // s.start();
}