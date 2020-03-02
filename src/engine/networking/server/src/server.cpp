#include "server/inc/server.hpp"

#include <functional>

using namespace bnetw::networking;

server::server(unsigned short port)
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
}

server::~server()
{
  io_context_.stop();
  acceptor_.close();
}

void server::start()
{
    // Start an accept operation for a new connection.
    auto new_conn = boost::make_shared<connection>(acceptor_.get_executor().context());

    auto hash_string = "secret server data"; 
    auto sID = std::hash<std::string>{}(hash_string);
    server_id_ = sID;

    std::cout << "SERVER ID: " << server_id_ << std::endl;

    acceptor_.async_accept(new_conn->socket(), 
                           [this, new_conn](const boost::system::error_code e)
                           {
                             handle_accept(e, new_conn);
                           }
    );

    io_context_.run();
}

void server::handle_accept(const boost::system::error_code& e, connection_ptr conn)
{
  if (!e)
  {
    /* Generate UserID here as hash based on connection data */
    auto hash_string = conn->socket().remote_endpoint().address().to_string() + std::to_string(conn->socket().remote_endpoint().port()); 
    auto uID = std::hash<std::string>{}(hash_string);
    // std::cout << "Generated userID: " << uID << '\n';
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

    do_async_read_from_user(userID);
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
  }
  else
  {
    std::cerr << "handle_write - error:  " << e.message() << std::endl;
  }
  
}

void server::do_async_read_from_user(api::UserID userID)
{
    auto& m = messages_[userID];
    auto& conn = clients_[userID];
    conn->async_read(m, 
                      [this, userID](boost::system::error_code e,  std::size_t)
                      {
                        handle_read(e, userID);
                      }
    );
}

void server::handle_read(const boost::system::error_code& e, api::UserID userID)
{
  if (!e)
  {
    std::cout << "messages_ size = " << messages_.size() << std::endl;
    std::cout << "FROM " << userID << " ";
    auto & msg = messages_[userID];

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

        auto & recv_conn = clients_[msg.receiverID()];
        
        recv_conn->async_write(msg,
                              [this, userID](boost::system::error_code e,  std::size_t)
                              {
                                handle_write(e, userID);
                              }
        );

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
            std::cout << "Sending to: " <<  c.first << std::endl;
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

    do_async_read_from_user(userID);
  }
  else
  {
    std::cerr << "handle_read - error:  " << e.message() << std::endl;
  }
}