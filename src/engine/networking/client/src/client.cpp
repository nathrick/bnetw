#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include <sstream>

#include "gsdk_api.hpp"
#include "connection.hpp" // Must come before boost/serialization headers.
#include "messages/message.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/thread/thread.hpp>

using namespace gsdk::networking;

using uID = gsdk::api::UserID;

using recvMsgCallback = void(gsdk::api::Client::*)(uID, const std::string &);

class gsdk::api::Client::ClientImpl
{
public:
    ClientImpl();
    ~ClientImpl();

    bool login();
    // virtual void peekReceivedMessage(uID senderID, const std::string & data) = 0;
    void sendBroadcastMessage(const std::string & msg);
    void sendServerMessage(const std::string & msg);
    void sendMessage(uID userID, const std::string & msg);

    uID id() const { return id_; }

    void setMessageHandler(gsdk::api::Client * client) { client_ = client; }

private:

  static inline std::string const HOST = "localhost";
  static inline std::string const SERVICE = "8888";

  boost::asio::io_context io_context_;
  connection connection_;
  boost::thread context_thread_;
  message message_;
  uID id_;
  uID server_id_;
  gsdk::api::Client * client_;

  void handle_connect(const boost::system::error_code& e);
  void handle_read(const boost::system::error_code& e);
  void handle_write(const boost::system::error_code& e);

  void send(uID userID, MESSAGE_TYPE type, const std::string & data);
  void doAsyncRead();
  void run_context_thread();

};

gsdk::api::Client::ClientImpl::ClientImpl()
        : connection_(io_context_)
{
}

gsdk::api::Client::ClientImpl::~ClientImpl()
{
    io_context_.stop();
    
    if(context_thread_.joinable())
        context_thread_.join();
    
    connection_.socket().close();
}

void gsdk::api::Client::ClientImpl::run_context_thread()
{
    while(!io_context_.stopped())
    {
        io_context_.run();
    }
}


void gsdk::api::Client::ClientImpl::handle_connect(const boost::system::error_code& e)
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

void gsdk::api::Client::ClientImpl::handle_read(const boost::system::error_code& e)
{
    if (!e)
    {
        //(static_cast<gsdk::api::Client*>(this)->*rmc_)(message_.senderID(), message_.data());
        client_->peekReceivedMessage(message_.senderID(), message_.data());
        doAsyncRead();
    }
    else
    {
      std::cerr << "handle_read - error:  " << e.message() << std::endl;
    }
}

void gsdk::api::Client::ClientImpl::handle_write(const boost::system::error_code& e)
{
    if(!e)
    {
    }
    else
    {
        std::cerr << "handle_write - error:  " << e.message() << std::endl;
    }
    
}

void gsdk::api::Client::ClientImpl::send(uID userID, MESSAGE_TYPE type, const std::string & data)
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

void gsdk::api::Client::ClientImpl::doAsyncRead()
{
    connection_.async_read(message_, 
                            [this](boost::system::error_code e,  std::size_t)
                            {
                                handle_read(e);
                            }
    );
}

bool gsdk::api::Client::ClientImpl::login()
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
    context_thread_ = boost::thread(&gsdk::api::Client::ClientImpl::run_context_thread, this); 

    return true;
}

void gsdk::api::Client::ClientImpl::sendBroadcastMessage(const std::string & msg)
{
    assert(server_id_.isValid());
    send(server_id_, MESSAGE_TYPE::TO_ALL, msg);
}

void gsdk::api::Client::ClientImpl::sendServerMessage(const std::string & msg)
{
    assert(server_id_.isValid());
    send(server_id_, MESSAGE_TYPE::TO_SERVER, msg);
}

void gsdk::api::Client::ClientImpl::sendMessage(uID userID, const std::string & msg)
{
    assert(server_id_.isValid());
    send(userID, MESSAGE_TYPE::TO_USER, msg);
}

gsdk::api::Client::Client()
    : pimpl_(std::make_unique<gsdk::api::Client::ClientImpl>())
{
    pimpl_->setMessageHandler(this);
}

gsdk::api::Client::~Client() = default;

bool gsdk::api::Client::login()
{
    return pimpl_->login();
}

void gsdk::api::Client::sendBroadcastMessage(const std::string & msg)
{
    pimpl_->sendBroadcastMessage(msg);
}

void gsdk::api::Client::sendServerMessage(const std::string & msg)
{
    pimpl_->sendServerMessage(msg);
}

void gsdk::api::Client::sendMessage(api::UserID userID, const std::string & msg)
{
    pimpl_->sendMessage(userID, msg);
}

uID gsdk::api::Client::id() const
{
    return pimpl_->id();
}