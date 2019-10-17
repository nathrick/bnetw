#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include "message.hpp"
#include "connection.hpp" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>

namespace gsdk
{

class client
{
public:
    client(boost::asio::io_context& io_context, const std::string& host, const std::string& service);

    /// Handle completion of a connect operation.
    void handle_connect(const boost::system::error_code& e);

    void handle_read(const boost::system::error_code& e);

    void handle_write(const boost::system::error_code& e);

private:
  /// The connection to the server.
  connection connection_;
  
  /// Last received message
  message message_;

  void wait_for_message();

};

} // namespace gsdk


#endif // CLIENT_HPP
