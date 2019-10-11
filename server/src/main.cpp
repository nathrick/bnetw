#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "connection.hpp"

void print(const boost::system::error_code& /*e*/)
{
  std::cout << "Async" << std::endl;
}

int main()
{
    boost::asio::io_context io;

    boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
    t.async_wait(&print);

    std::cout << "Main" << std::endl;

    io.run();

    return 0;
}

