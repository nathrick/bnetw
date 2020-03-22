#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include <sstream>

#include "bnetw_api.hpp"
#include "connection.hpp" // Must come before boost/serialization headers.
#include "messages/message.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/thread/thread.hpp>

using namespace bnetw::networking;

using uID = bnetw::api::UserID;

using recvMsgCallback = void (bnetw::api::Client::*)(uID, const std::string &);

class bnetw::api::Client::ClientImpl
{
public:
    ClientImpl(bnetw::api::Client *client);
    ~ClientImpl();

    bool login();
    void sendBroadcastMessage(const std::string &msg);
    void sendServerMessage(const std::string &msg);
    void sendMessage(uID userID, const std::string &msg);
    void disconnect();

    uID id() const { return id_; }

private:
    static inline std::string const HOST = "localhost";
    static inline std::string const SERVICE = "8888";

    boost::asio::io_context io_context_;
    connection<boost::asio::ip::tcp::socket> connection_;
    boost::thread context_thread_;
    message message_;
    uID id_;
    uID server_id_;
    bnetw::api::Client *client_;

    void handle_connect(const boost::system::error_code &e);
    void handle_read(const boost::system::error_code &e);
    void handle_write(const boost::system::error_code &e);

    void send(uID userID, MESSAGE_TYPE type, const std::string &data);
    void doAsyncRead();
    void run_context_thread();
};

bnetw::api::Client::ClientImpl::ClientImpl(bnetw::api::Client *client)
    : connection_(io_context_), client_(client)
{
}

bnetw::api::Client::ClientImpl::~ClientImpl()
{
    if (context_thread_.joinable())
        context_thread_.join();

    io_context_.stop();
}

void bnetw::api::Client::ClientImpl::run_context_thread()
{
    while (!io_context_.stopped())
    {
        io_context_.run();
    }
}

void bnetw::api::Client::ClientImpl::handle_connect(const boost::system::error_code &e)
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

void bnetw::api::Client::ClientImpl::handle_read(const boost::system::error_code &e)
{
    if (!e)
    {
        client_->peekReceivedMessage(message_.senderID(), message_.data());
        doAsyncRead();
    }
    else
    {
        if (e.value() == boost::system::errc::no_such_file_or_directory)
        {
            std::cout << "Server " << server_id_ << " disconnected..." << std::endl;
        }
        else if(e.value() == boost::system::errc::operation_canceled)
        {
            std::cout << "Disconnected!" << std::endl;
        }
        else
        {
            std::cerr << "handle_read - error:  " << e.message() << std::endl;
        }
    }
}

void bnetw::api::Client::ClientImpl::handle_write(const boost::system::error_code &e)
{
    if (!e)
    {
    }
    else
    {
        std::cerr << "handle_write - error:  " << e.message() << std::endl;
    }
}

void bnetw::api::Client::ClientImpl::send(uID userID, MESSAGE_TYPE type, const std::string &data)
{
    if (!userID.isValid())
    {
        std::cerr << "Invalid client id\n";
        return;
    }

    if ((type == MESSAGE_TYPE::UNKNOWN) || (type == MESSAGE_TYPE::REGISTER_USER))
    {
        std::cerr << "Unhandled message type\n";
        return;
    }

    if (data.empty())
    {
        std::cerr << "Empty data\n";
        return;
    }

    /* Prepare and send message to server */
    message m{type, id_, userID};
    m.setData(data);

    connection_.async_write(m,
                            [this](boost::system::error_code e, std::size_t) {
                                handle_write(e);
                            });
}

void bnetw::api::Client::ClientImpl::doAsyncRead()
{
    connection_.async_read(message_,
                           [this](boost::system::error_code e, std::size_t) {
                               handle_read(e);
                           });
}

bool bnetw::api::Client::ClientImpl::login()
{
    // Resolve the host name into an IP address.
    boost::asio::ip::tcp::resolver resolver(connection_.socket().get_io_context());
    boost::asio::ip::tcp::resolver::query query(HOST, SERVICE);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    boost::system::error_code e;
    boost::asio::connect(connection_.socket(), endpoint_iterator, e);

    if (e)
    {
        std::cerr << "Connection error\n";
        return false;
    }

    connection_.read(message_, e);

    if (e)
    {
        std::cerr << "Read error\n";
        return false;
    }
    else
    {
        if (message_.type() == MESSAGE_TYPE::REGISTER_USER)
        {
            std::istringstream iss_data(message_.data());
            size_t id;
            iss_data >> id;
            id_ = id;

            server_id_ = message_.senderID();

            std::cout << "Login success, received user id: " << id_ << '\n';
        }
        else
        {
            std::cerr << "Login failed, incorrect message type: " << to_underlying(message_.type()) << '\n';
            return false;
        }
    }

    doAsyncRead();
    context_thread_ = boost::thread(&bnetw::api::Client::ClientImpl::run_context_thread, this);

    return true;
}

void bnetw::api::Client::ClientImpl::sendBroadcastMessage(const std::string &msg)
{
    assert(server_id_.isValid());
    send(server_id_, MESSAGE_TYPE::TO_ALL, msg);
}

void bnetw::api::Client::ClientImpl::sendServerMessage(const std::string &msg)
{
    assert(server_id_.isValid());
    send(server_id_, MESSAGE_TYPE::TO_SERVER, msg);
}

void bnetw::api::Client::ClientImpl::sendMessage(uID userID, const std::string &msg)
{
    assert(server_id_.isValid());
    send(userID, MESSAGE_TYPE::TO_USER, msg);
}

void bnetw::api::Client::ClientImpl::disconnect()
{
    boost::system::error_code e;
    connection_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, e);
    connection_.socket().close();
}

bnetw::api::Client::Client()
    : pimpl_(std::make_unique<bnetw::api::Client::ClientImpl>(this))
{
}

bnetw::api::Client::~Client() = default;

bool bnetw::api::Client::login()
{
    return pimpl_->login();
}

void bnetw::api::Client::sendBroadcastMessage(const std::string &msg)
{
    pimpl_->sendBroadcastMessage(msg);
}

void bnetw::api::Client::sendServerMessage(const std::string &msg)
{
    pimpl_->sendServerMessage(msg);
}

void bnetw::api::Client::sendMessage(api::UserID userID, const std::string &msg)
{
    pimpl_->sendMessage(userID, msg);
}

void bnetw::api::Client::disconnect()
{
    pimpl_->disconnect();
}

uID bnetw::api::Client::id() const
{
    return pimpl_->id();
}
