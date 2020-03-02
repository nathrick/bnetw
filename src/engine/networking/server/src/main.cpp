#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "connection.hpp"
#include "server/inc/server.hpp"

int main(int argc, char* argv[])
{
    try
    {
      // Check command line arguments.
      if (argc != 2)
      {
        std::cerr << "Usage: " << argv[0]  << " <port>" << std::endl;
        return 1;
      }
      unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);

      bnetw::networking::server server(port);
      server.start();
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

    return 0;
}

