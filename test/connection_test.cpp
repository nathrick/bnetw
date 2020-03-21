#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/thread/thread.hpp>

#include "connection.hpp"

using namespace bnetw::networking;

class connection_test : public ::testing::Test
{
public:
    connection_test() : io_context_()
    {
    }

    void SetUp()
    {
        std::cout << __func__ << std::endl;
    }

    void TearDown()
    {
        std::cout << __func__ << std::endl;
    }

protected:
    boost::asio::io_context io_context_;
};

// TEST_F(connection_test, SimpleAsyncWrite)
// {
//     //std::string test_data = "Asia";
//     wd_ = "Asia";

//     test_connection_->async_write(wd_, [](boost::system::error_code e, std::size_t) { std::cout << "SimpleAsyncWrite: async_write - " << e.message() << std::endl; });

//     io_context_.run();
// }

// TEST_F(connection_test, EmptyDataAsyncWrite)
// {
//     std::string test_data = "";

//     test_connection_->async_write(test_data, [](boost::system::error_code e, std::size_t) { std::cout << "EmptyDataAsyncWrite: async_write - " << e.message() << std::endl; });
// }

TEST_F(connection_test, SimpleAsyncWriteRead)
{
    auto conn1 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    auto conn2 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    std::string written_data = "Asia";
    std::string read_data;

    boost::asio::local::connect_pair(conn1->socket(), conn2->socket());

    conn1->async_read(read_data, [](boost::system::error_code e, std::size_t) { std::cout << "SimpleAsyncWriteRead: async_read - " << e.message() << std::endl; });
    conn2->async_write(written_data, [](boost::system::error_code e, std::size_t) { std::cout << "SimpleAsyncWriteRead: async_write - " << e.message() << std::endl; });

    io_context_.run();

    ASSERT_STREQ(written_data.c_str(), read_data.c_str());
}

TEST_F(connection_test, EmptyAsyncWriteRead)
{
    auto conn1 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    auto conn2 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    std::string written_data = "";
    std::string read_data;

    boost::asio::local::connect_pair(conn1->socket(), conn2->socket());

    conn1->async_read(read_data, [](boost::system::error_code e, std::size_t) { std::cout << "SimpleAsyncWriteRead: async_read - " << e.message() << std::endl; });
    conn2->async_write(written_data, [](boost::system::error_code e, std::size_t) { std::cout << "SimpleAsyncWriteRead: async_write - " << e.message() << std::endl; });

    io_context_.run();

    ASSERT_STREQ(written_data.c_str(), read_data.c_str());
}

TEST_F(connection_test, InvalidTargetTypeAsyncWriteRead)
{
    auto conn1 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    auto conn2 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    std::string written_data = "";
    int read_data;

    boost::asio::local::connect_pair(conn1->socket(), conn2->socket());

    conn1->async_read(read_data, [](boost::system::error_code e, std::size_t) { std::cout << "SimpleAsyncWriteRead: async_read - " << e.message() << std::endl; });
    conn2->async_write(written_data, [](boost::system::error_code e, std::size_t) { std::cout << "SimpleAsyncWriteRead: async_write - " << e.message() << std::endl; });

    io_context_.run();
}

TEST_F(connection_test, SimpleWriteRead)
{
    auto conn1 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    auto conn2 = boost::make_shared<connection<boost::asio::local::stream_protocol::socket>>(io_context_);
    std::string written_data = "Asia";
    std::string read_data;
    boost::system::error_code e;
    boost::thread read_thread;

    boost::asio::local::connect_pair(conn1->socket(), conn2->socket());

    read_thread = boost::thread([&conn1, &read_data] {
        boost::system::error_code e;
        conn1->read(read_data, e);
    });

    conn2->write(written_data, e);

    io_context_.run();

    if (read_thread.joinable())
    {
        read_thread.join();
    }

    ASSERT_STREQ(written_data.c_str(), read_data.c_str());
}