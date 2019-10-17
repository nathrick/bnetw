#include "client/inc/client.hpp"

using namespace gsdk;

client::client(boost::asio::io_context& io_context, const std::string& host, const std::string& service)
    : connection_(io_context)
{
    // Resolve the host name into an IP address.
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::query query(host, service);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Start an asynchronous connect operation.
    boost::asio::async_connect(connection_.socket(), endpoint_iterator,
        boost::bind(&client::handle_connect, this, boost::asio::placeholders::error));
}


void client::handle_connect(const boost::system::error_code& e)
{
    if (!e)
    {
        auto m = message(MESSAGE_TYPE::TEST_TYPE_1);
        connection_.async_write(m, boost::bind(&client::handle_write, this, boost::asio::placeholders::error));

        wait_for_message();
    }
    else
    {
      // An error occurred. Log it and return. Since we are not starting a new
      // operation the io_context will run out of work to do and the client will
      // exit.
      std::cerr << e.message() << std::endl;
    }
}

void client::handle_read(const boost::system::error_code& e)
{
    std::cout << "handle_read - ";
    
    message_.printType();

    wait_for_message();
}

void client::handle_write(const boost::system::error_code& e)
{

}

void client::wait_for_message()
{
    connection_.async_read(message_, boost::bind(&client::handle_read, this, boost::asio::placeholders::error));
}