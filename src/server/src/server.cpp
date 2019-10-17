#include "server/inc/server.hpp"

using namespace gsdk;

server::server(boost::asio::io_context& io_context, unsigned short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    // Start an accept operation for a new connection.
    connection_ptr new_conn(new connection(acceptor_.get_executor().context()));
    acceptor_.async_accept(new_conn->socket(), boost::bind(&gsdk::server::handle_accept, this, boost::asio::placeholders::error, new_conn));
}

void server::handle_accept(const boost::system::error_code& e, connection_ptr conn)
{
  if (!e)
  {
    // auto m = message(MESSAGE_TYPE::TEST_TYPE_2);
    // conn->async_write(m, boost::bind(&server::handle_write, this, boost::asio::placeholders::error, conn, m));

    clients_.insert( std::make_pair( conn->socket().remote_endpoint(), message(MESSAGE_TYPE::UNKNOWN)) );
    //std::cout << "Client: " << conn->socket().remote_endpoint().address().to_string() << " : " << conn->socket().remote_endpoint().port() << "\n";

    auto& m = clients_.at(conn->socket().remote_endpoint());
    conn->async_read( m,  boost::bind(&server::handle_read, this, boost::asio::placeholders::error, conn) );
  
  }

  // Start an accept operation for a new connection.
  connection_ptr new_conn(new connection(acceptor_.get_executor().context()));
  acceptor_.async_accept(new_conn->socket(), boost::bind(&server::handle_accept, this, boost::asio::placeholders::error, new_conn));
}

void server::handle_write(const boost::system::error_code& e, connection_ptr conn, message& m)
{
    static_cast<void>(e);
    static_cast<void>(conn);

    //m.printType();
}

void server::handle_read(const boost::system::error_code& e, connection_ptr conn)
{
  if (!e)
  {
    auto new_message = clients_.at(conn->socket().remote_endpoint());

    new_message.printType();

    switch (new_message.type())
    {
    case MESSAGE_TYPE::UNKNOWN:
      /* code */
      break;
    case MESSAGE_TYPE::CONNECTION_ACK:
      /* code */
      break;      
    case MESSAGE_TYPE::TEST_TYPE_1:
    {
      auto m = message(MESSAGE_TYPE::CONNECTION_ACK);
      conn->async_write(m, boost::bind(&server::handle_write, this, boost::asio::placeholders::error, conn, m));
      break;
    }
    case MESSAGE_TYPE::TEST_TYPE_2:
      /* code */
      break;
    default:
      break;
    }
  }
  
  auto m = clients_.at(conn->socket().remote_endpoint());
  conn->async_read( m,  boost::bind(&server::handle_read, this, boost::asio::placeholders::error, conn) );
}