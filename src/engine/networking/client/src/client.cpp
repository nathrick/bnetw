#include "engine/networking/client/inc/client.hpp"

using namespace gsdk::networking;

client::client(boost::asio::io_context& io_context, const std::string& host, const std::string& service)
    : connection_(io_context)
{
    // Resolve the host name into an IP address.
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::query query(host, service);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Start an asynchronous connect operation.
    boost::asio::async_connect(connection_.socket(), endpoint_iterator,
                                [this](boost::system::error_code e, boost::asio::ip::tcp::resolver::iterator)
                                {
                                    handle_connect(e);
                                }
    );
}


void client::handle_connect(const boost::system::error_code& e)
{
    if (!e)
    {
        connection_.async_read(message_, 
                                [this](boost::system::error_code e,  std::size_t)
                                {
                                    handle_read(e);
                                }
        );
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
    if (!e)
    {
        std::cout << "handle_read - data:  " << message_.data() << '\n';

        connection_.async_read(message_, 
                                [this](boost::system::error_code e,  std::size_t)
                                {
                                    handle_read(e);
                                }
        );
    }
    else
    {
      std::cerr << "handle_read - error:  " << e.message() << std::endl;
    }
}

void client::handle_write(const boost::system::error_code& e)
{

}