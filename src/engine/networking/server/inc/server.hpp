#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>

#include "bnetw_api.hpp"
#include "messages/message.hpp"
#include "connection.hpp" // Must come before boost/serialization headers.

#include <boost/serialization/vector.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/thread/thread.hpp>

namespace bnetw
{

namespace networking
{

class server
{
public:
    server(unsigned short port);
    ~server();
    void start();
    void handle_accept(const boost::system::error_code& e, connection_ptr conn);
    void handle_write(const boost::system::error_code& e, api::UserID userID);
    void handle_read(const boost::system::error_code& e, api::UserID userID);

private:
  /// The acceptor object used to accept incoming socket connections.
  boost::asio::io_context io_context_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::map<api::UserID, connection_ptr> clients_;
  std::map<api::UserID, message> messages_;
  api::UserID server_id_;

  void do_async_read_from_user(api::UserID userID);

};

} // namespace networking

} // namespace bnetw

#endif // SERVER_HPP
