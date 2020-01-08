#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "engine/networking/connection.hpp"
#include "engine/networking/client/inc/client.hpp"

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    // if (argc != 3)
    // {
    //   std::cerr << "Usage: client <host> <port>" << std::endl;
    //   return 1;
    // }

    //boost::asio::io_context io_context;
    gsdk::networking::client client;

    if(client.login())
    {
      char c;

      do
      {
        std::cout << "Choose sth:" << std::endl;
        std::cin >> c;

        if(c == 'm')
        {
          client.sendBroadcastMessage( std::string("I am alive: " + client.id().toString()) );
        }
        else if(c == 'u')
        {
          size_t recv_uid;
          std::cout << "Enter receiver user ID:\t";
          std::cin >> recv_uid;

          client.sendMessage(gsdk::api::UserID {recv_uid}, std::string("Direct msg from: " + client.id().toString()));
        }

      } while (c != 'c');

    }

  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

