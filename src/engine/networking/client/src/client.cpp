#include "engine/networking/client/inc/client.hpp"

#include <sstream>

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
        doRead();
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
        switch (message_.type())
        {
        case MESSAGE_TYPE::UNKNOWN:
            /* code */
            std::cout << "Received message UNKNOWN\n";
            break;
        case MESSAGE_TYPE::REGISTER_USER:
        {
            std::istringstream iss(message_.data());
            size_t id; iss >> id; id_ = id;
            std::cout << "Received message REGISTER_USER, received user id: "<< id_ << '\n';

            sendMessage(message_.senderID(), MESSAGE_TYPE::TO_ALL, std::string("User " + message_.data() + " is ACTIVE!"));

            break;      
        }
        case MESSAGE_TYPE::TO_USER:
        {
            std::cout << "Received message TO_USER\n";
            break;
        }
        case MESSAGE_TYPE::TO_ALL:
        {
            std::cout << "Received message TO_ALL: " << message_.data() << '\n';
            
            doRead();

            break;
        }
        default:
            break;
        }
    }
    else
    {
      std::cerr << "handle_read - error:  " << e.message() << std::endl;
    }
}

void client::handle_write(const boost::system::error_code& e)
{
    if(!e)
    {
        doRead();
    }
    else
    {
        std::cerr << "handle_write - error:  " << e.message() << std::endl;
    }
    
}

void client::sendMessage(api::UserID userID, MESSAGE_TYPE type, const std::string & data)
{
    if( id_ == 0 )
    {
        std::cerr << "Invalid client id\n";
        return;
    }

    if( (type == MESSAGE_TYPE::UNKNOWN) || (type == MESSAGE_TYPE::REGISTER_USER) )
    {
        std::cerr << "Unhandled message type\n";
        return;
    }

    if( data.empty() )
    {
        std::cerr << "Empty data\n";
        return;
    }

    /* Prepare and send message to server */
    message m {type, id_, userID};
    m.setData( data );

    connection_.async_write(m, 
                        [this](boost::system::error_code e,  std::size_t)
                        {
                            handle_write(e);
                        }
    );

}

void client::doRead()
{
    connection_.async_read(message_, 
                            [this](boost::system::error_code e,  std::size_t)
                            {
                                handle_read(e);
                            }
    );
}