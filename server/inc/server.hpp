#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>
#include "message.hpp"
#include "connection.hpp" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>

namespace gsdk_server
{

class server
{
public:
    server(boost::asio::io_context& io_context, unsigned short port);

    /// Handle completion of a accept operation.
    void handle_accept(const boost::system::error_code& e, connection_ptr conn);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e, connection_ptr conn, message& m);

private:
  /// The acceptor object used to accept incoming socket connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The data to be sent to each client.
  std::vector<int> clients_;
};

} // namespace gsdk_server

#endif // SERVER_HPP
