#include "engine/networking/server/inc/server.hpp"
#include "engine/networking/messages/message.hpp"

#include <functional>

using namespace gsdk::networking;

server::server(boost::asio::io_context& io_context, unsigned short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    // Start an accept operation for a new connection.
    connection_ptr new_conn(new connection(acceptor_.get_executor().context()));
    acceptor_.async_accept(new_conn->socket(), boost::bind(&server::handle_accept, this, boost::asio::placeholders::error, new_conn));
}

void server::handle_accept(const boost::system::error_code& e, connection_ptr conn)
{
  if (!e)
  {
    /* Generate UserID here as hash based on connection data */
    auto hash_string = conn->socket().remote_endpoint().address().to_string() + std::to_string(conn->socket().remote_endpoint().port()); 
    auto uID = std::hash<std::string>{}(hash_string);
    std::cout << "Generated userID: " << uID << '\n';
    api::UserID userID;
    userID.id_ = uID;
    
    /* Add generated UserID to connected clients */
    clients_[userID] = conn->socket().remote_endpoint();
    //clients_.insert( std::make_pair( conn->socket().remote_endpoint(), api::UserID{} ) );
    //std::cout << "Client: " << conn->socket().remote_endpoint().address().to_string() << " : " << conn->socket().remote_endpoint().port() << "\n";

    //conn->async_read( m,  boost::bind(&server::handle_read, this, boost::asio::placeholders::error, conn) );

    /* Prepare and send message to client */
    message m {MESSAGE_TYPE::REGISTER_USER, server_id_, userID};
    m.setData(std::to_string(uID));
    conn->async_write( m,  boost::bind(&server::handle_write, this, boost::asio::placeholders::error, conn) );
  }

  // Start an accept operation for a new connection.
  connection_ptr new_conn(new connection(acceptor_.get_executor().context()));
  acceptor_.async_accept(new_conn->socket(), boost::bind(&server::handle_accept, this, boost::asio::placeholders::error, new_conn));
}

void server::handle_write(const boost::system::error_code& e, connection_ptr conn)
{
    static_cast<void>(e);
    static_cast<void>(conn);

    //m.printType();
}

void server::handle_read(const boost::system::error_code& e, connection_ptr conn)
{
  if (!e)
  {
  //   auto client_endpoint = conn->socket().remote_endpoint();
  //   auto new_message = clients_.at(client_endpoint);

  //   std::cout << "Message from client: " << client_endpoint.address().to_string() << " : " << client_endpoint.port() << "\n";
  //   new_message.printType();

  //   switch (new_message.type())
  //   {
  //   case MESSAGE_TYPE::UNKNOWN:
  //     /* code */
  //     break;
  //   case MESSAGE_TYPE::CONNECTION_ACK:
  //     /* code */
  //     break;      
  //   case MESSAGE_TYPE::TEST_TYPE_1:
  //   {
  //     auto m = message(MESSAGE_TYPE::CONNECTION_ACK);
  //     conn->async_write(m, boost::bind(&server::handle_write, this, boost::asio::placeholders::error, conn, m));
  //     break;
  //   }
  //   case MESSAGE_TYPE::TEST_TYPE_2:
  //     /* code */
  //     break;
  //   default:
  //     break;
  //   }
  }
  
  // auto m = clients_.at(conn->socket().remote_endpoint());
  // conn->async_read( m,  boost::bind(&server::handle_read, this, boost::asio::placeholders::error, conn) );
}