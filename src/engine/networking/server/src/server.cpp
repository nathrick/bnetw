#include "engine/networking/server/inc/server.hpp"

#include <functional>

using namespace gsdk::networking;

server::server(boost::asio::io_context& io_context, unsigned short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    // Start an accept operation for a new connection.
    auto new_conn = boost::make_shared<connection>(acceptor_.get_executor().context());
    acceptor_.async_accept(new_conn->socket(), 
                           [this, new_conn](const boost::system::error_code e)
                           {
                             handle_accept(e, new_conn);
                           }
    );
}

void server::handle_accept(const boost::system::error_code& e, connection_ptr conn)
{
  if (!e)
  {
    /* Generate UserID here as hash based on connection data */
    auto hash_string = conn->socket().remote_endpoint().address().to_string() + std::to_string(conn->socket().remote_endpoint().port()); 
    auto uID = std::hash<std::string>{}(hash_string);
    std::cout << "Generated userID: " << uID << '\n';
    api::UserID userID{uID};
    
    /* Add generated UserID to connected clients */
    clients_[userID] = conn;

    /* Prepare and send message to client */
    message m {MESSAGE_TYPE::REGISTER_USER, server_id_, userID};
    m.setData(std::to_string(uID));

    conn->async_write(m, 
                      [this, userID](boost::system::error_code e,  std::size_t)
                      {
                        handle_write(e, userID);
                      }
    );
  }

  // Start an accept operation for a new connection.
  auto new_conn = boost::make_shared<connection>(acceptor_.get_executor().context());
  acceptor_.async_accept(new_conn->socket(), 
                         [this, new_conn](const boost::system::error_code e)
                         {
                            handle_accept(e, new_conn);
                         }
  );
}

void server::handle_write(const boost::system::error_code& e, api::UserID userID)
{
  if(!e)
  {
    auto& m = messages_[userID];
    auto& conn = clients_[userID];
    conn->async_read(m, 
                      [this, conn, userID](boost::system::error_code e,  std::size_t)
                      {
                        handle_read(e, userID);
                      }
    );
  }
  else
  {
    std::cerr << "handle_write - error:  " << e.message() << std::endl;
  }
  
}

void server::handle_read(const boost::system::error_code& e, api::UserID userID)
{
  if (!e)
  {
    auto msg = messages_[userID];

    switch (msg.type())
    {
      case MESSAGE_TYPE::UNKNOWN:
        /* code */
        std::cout << "Received message UNKNOWN\n";
        break;
      case MESSAGE_TYPE::TO_SERVER:
        /* code */
        std::cout << "Received message TO_SERVER\n";
        break;      
      case MESSAGE_TYPE::TO_USER:
      {
        std::cout << "Received message TO_USER\n";
        break;
      }
      case MESSAGE_TYPE::TO_ALL:
        {
          std::cout << "Received message TO_ALL\n";

          for(auto & c : clients_)
          {
            /* Prepare and send message to all users */
            if( !(c.first == msg.senderID()) )
            {
              message m { msg.type(), msg.receiverID(), c.first };
              m.setData( msg.data() );

              c.second->async_write(m, 
                                    [this, userID](boost::system::error_code e,  std::size_t)
                                    {
                                      handle_write(e, userID);
                                    }
              );
            }
          }

          break;
        }
      default:
        break;
    }
  }
  else
  {
    std::cerr << "handle_read - error:  " << e.message() << std::endl;
  }
}