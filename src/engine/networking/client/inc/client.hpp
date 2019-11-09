#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include "engine/networking/connection.hpp" // Must come before boost/serialization headers.
#include "engine/networking/messages/message.hpp"
#include <boost/serialization/vector.hpp>

namespace gsdk
{

namespace networking
{

class client
{
public:
    client(boost::asio::io_context& io_context, const std::string& host, const std::string& service);

    void sendMessage(api::UserID userID, MESSAGE_TYPE type, const std::string & data);

private:
  connection connection_;
  message message_;
  api::UserID id_;

  void handle_connect(const boost::system::error_code& e);
  void handle_read(const boost::system::error_code& e);
  void handle_write(const boost::system::error_code& e);

  void doRead();

};

} // namespace networking

} // namespace gsdk


#endif // CLIENT_HPP
