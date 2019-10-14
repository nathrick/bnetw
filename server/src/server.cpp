#include "server.hpp"
#include "message.hpp"

using namespace gsdk_server;

server::server(boost::asio::io_context& io_context, unsigned short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    // Start an accept operation for a new connection.
    connection_ptr new_conn(new connection(acceptor_.get_executor().context()));
    acceptor_.async_accept(new_conn->socket(), boost::bind(&gsdk_server::server::handle_accept, this, boost::asio::placeholders::error, new_conn));
}

void server::handle_accept(const boost::system::error_code& e, connection_ptr conn)
{
  if (!e)
  {
    auto m = message(MESSAGE_TYPE::TEST_TYPE_2);
    // Successfully accepted a new connection. Send the list of stocks to the
    // client. The connection::async_write() function will automatically
    // serialize the data structure for us.
    conn->async_write(m, boost::bind(&server::handle_write, this, boost::asio::placeholders::error, conn, m));
  }

  // Start an accept operation for a new connection.
  connection_ptr new_conn(new connection(acceptor_.get_executor().context()));
  acceptor_.async_accept(new_conn->socket(), boost::bind(&server::handle_accept, this, boost::asio::placeholders::error, new_conn));
}

void server::handle_write(const boost::system::error_code& e, connection_ptr conn, message& m)
{
    static_cast<void>(e);
    static_cast<void>(conn);

    m.printType();
}
