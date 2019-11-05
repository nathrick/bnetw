#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>

#include "api/api.hpp"
#include "engine/networking/messages/message.hpp"
#include "engine/networking/connection.hpp" // Must come before boost/serialization headers.

#include <boost/serialization/vector.hpp>
#include <boost/asio/basic_socket.hpp>

namespace gsdk
{

namespace networking
{

class server
{
public:
    server(boost::asio::io_context& io_context, unsigned short port);

    /// Handle completion of a accept operation.
    void handle_accept(const boost::system::error_code& e, connection_ptr conn);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e, api::UserID userID);

    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code& e, api::UserID userID);

private:
  /// The acceptor object used to accept incoming socket connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  std::map<api::UserID, connection_ptr> clients_;

  std::map<api::UserID, message> messages_;

  api::UserID server_id_;
};

} // namespace networking

} // namespace gsdk

#endif // SERVER_HPP
