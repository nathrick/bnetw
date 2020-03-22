#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "server/inc/server.hpp"

using namespace bnetw::networking;

class server_test : public ::testing::Test
{
public:
    server_test()
    {
    }

    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

TEST_F(server_test, ConstructorTest)
{
    unsigned short port_num = 888;
    server s(port_num);
}

/*
TEST_F(server_test, ConnectTest)
{
    unsigned short port_num = 888;
    server s(port_num);
    boost::asio::io_context io_context;
    auto conn1 = boost::make_shared<connection<boost::asio::ip::tcp::socket>>(io_context);

    boost::asio::ip::tcp::resolver resolver(conn1->socket().get_io_context());
    boost::asio::ip::tcp::resolver::query query("localhost", std::to_string(port_num));
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::system::error_code e;
    boost::thread server_thread;

    server_thread = boost::thread([&s] {
        s.start();
    });

    boost::asio::connect(conn1->socket(), endpoint_iterator, e);

    io_context.run();

    server_thread.interrupt();

    ASSERT_EQ(e, boost::system::errc::success);
}
*/
