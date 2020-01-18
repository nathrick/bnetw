#include "engine/networking/client/inc/client.hpp"

#include <sstream>

using namespace gsdk::networking;

abstract_client::abstract_client()
        : connection_(io_context_)
{
}

abstract_client::~abstract_client()
{
    io_context_.stop();
    
    if(context_thread_.joinable())
        context_thread_.join();
    
    connection_.socket().close();
}

void abstract_client::run_context_thread()
{
    while(!io_context_.stopped())
    {
        io_context_.run();
    }
}


void abstract_client::handle_connect(const boost::system::error_code& e)
{
    if (!e)
    {
        doAsyncRead();
    }
    else
    {
      std::cerr << e.message() << std::endl;
    }
}

void abstract_client::handle_read(const boost::system::error_code& e)
{
    if (!e)
    {
        peekReceivedMessage(message_.senderID(), message_.data());
        doAsyncRead();
    }
    else
    {
      std::cerr << "handle_read - error:  " << e.message() << std::endl;
    }
}

void abstract_client::handle_write(const boost::system::error_code& e)
{
    if(!e)
    {
    }
    else
    {
        std::cerr << "handle_write - error:  " << e.message() << std::endl;
    }
    
}

void abstract_client::send(api::UserID userID, MESSAGE_TYPE type, const std::string & data)
{
    if( !userID.isValid() )
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

void abstract_client::doAsyncRead()
{
    connection_.async_read(message_, 
                            [this](boost::system::error_code e,  std::size_t)
                            {
                                handle_read(e);
                            }
    );
}

bool abstract_client::login()
{
    // Resolve the host name into an IP address.
    boost::asio::ip::tcp::resolver resolver(connection_.socket().get_io_context());
    boost::asio::ip::tcp::resolver::query query(HOST, SERVICE);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    boost::system::error_code e;
    boost::asio::connect(connection_.socket(), endpoint_iterator, e);

    if(e)
    {
        std::cerr << "Connection error\n";
        return false;
    }

    connection_.read(message_, e);

    if(e)
    {
        std::cerr << "Read error\n";
        return false;
    }
    else
    {
        if( message_.type() == MESSAGE_TYPE::REGISTER_USER ) 
        {
            std::istringstream iss_data(message_.data());
            size_t id; iss_data >> id; id_ = id;
            
            server_id_ = message_.senderID();

            std::cout << "Login success, received user id: " << id_  << '\n';
        }
        else
        {
            std::cerr << "Login failed, incorrect message type: " << to_underlying(message_.type()) << '\n';
            return false;
        }
    }

    doAsyncRead();
    context_thread_ = boost::thread(&abstract_client::run_context_thread, this); 

    return true;
}

void abstract_client::sendBroadcastMessage(const std::string & msg)
{
    assert(server_id_.isValid());
    send(server_id_, MESSAGE_TYPE::TO_ALL, msg);
}

void abstract_client::sendServerMessage(const std::string & msg)
{
    assert(server_id_.isValid());
    send(server_id_, MESSAGE_TYPE::TO_SERVER, msg);
}

void abstract_client::sendMessage(api::UserID userID, const std::string & msg)
{
    assert(server_id_.isValid());
    send(userID, MESSAGE_TYPE::TO_USER, msg);
}