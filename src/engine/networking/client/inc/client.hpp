#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include "engine/networking/connection.hpp" // Must come before boost/serialization headers.
#include "engine/networking/messages/message.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/thread/thread.hpp>

namespace gsdk
{

namespace networking
{

class client
{
public:
    client(/*boost::asio::io_context& io_context*/);
    ~client();

    bool login();

    void sendBroadcastMessage(const std::string & msg);
    void sendServerMessage(const std::string & msg);
    void sendMessage(api::UserID userID, const std::string & msg);  

    api::UserID id() const { return id_; }

private:

  static inline std::string const HOST = "localhost";
  static inline std::string const SERVICE = "8888";

  boost::asio::io_context io_context_;
  connection connection_;
  boost::thread context_thread_;
  message message_;
  api::UserID id_;
  api::UserID server_id_;

  void handle_connect(const boost::system::error_code& e);
  void handle_read(const boost::system::error_code& e);
  void handle_write(const boost::system::error_code& e);

  void send(api::UserID userID, MESSAGE_TYPE type, const std::string & data);
  void doAsyncRead();
  void run_context_thread();

};

} // namespace networking

} // namespace gsdk


#endif // CLIENT_HPP
