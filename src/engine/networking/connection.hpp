#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

namespace gsdk
{

namespace networking
{

class connection
{
public:
  connection(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  boost::asio::ip::tcp::socket& socket()
  {
    return socket_;
  }

  template <typename T, typename Handler>
  void async_write(const T& t, Handler handler)
  {
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << t;
    outbound_data_ = archive_stream.str();

    std::ostringstream header_stream;
    header_stream << std::setw(header_length) << std::hex << outbound_data_.size();
    if (!header_stream || header_stream.str().size() != header_length)
    {
      boost::system::error_code error(boost::asio::error::invalid_argument);
      boost::asio::post(socket_.get_io_service(), [handler, error]{ handler(error, 0); });
      return;
    }
    outbound_header_ = header_stream.str();

    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(outbound_header_));
    buffers.push_back(boost::asio::buffer(outbound_data_));
    boost::asio::async_write(socket_, buffers, handler);
  }

  template <typename T>
  void write(const T& t, boost::system::error_code& ec)
  {
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << t;
    outbound_data_ = archive_stream.str();

    std::ostringstream header_stream;
    header_stream << std::setw(header_length) << std::hex << outbound_data_.size();
    if (!header_stream || header_stream.str().size() != header_length)
    {
      ec = boost::asio::error::invalid_argument;
      return;
    }
    outbound_header_ = header_stream.str();

    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(outbound_header_));
    buffers.push_back(boost::asio::buffer(outbound_data_));
    boost::asio::write(socket_, buffers);
  }

  template <typename T, typename Handler>
  void async_read(T& t, Handler handler)
  {
    boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_),
                            [this, &t, handler](boost::system::error_code e, std::size_t)
                            {
                              handle_read_header(e, t, handler);
                            }
    );
  }

  template <typename T, typename Handler>
  void handle_read_header(const boost::system::error_code& e, T& t, Handler handler)
  {
    if (e)
    {
      handler(e, 0);
    }
    else
    {
      std::istringstream is(std::string(inbound_header_, header_length));
      std::size_t inbound_data_size = 0;
      if (!(is >> std::hex >> inbound_data_size))
      {
        boost::system::error_code error(boost::asio::error::invalid_argument);
        handler(e, 0);
        return;
      }

      inbound_data_.resize(inbound_data_size);
      boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_), 
                              [this, &t, handler](boost::system::error_code e, std::size_t)
                              {
                                handle_read_data(e, t, handler);
                              }  
      );
    }
  }

  template <typename T, typename Handler>
  void handle_read_data(const boost::system::error_code& e, T& t, Handler handler)
  {
    if (e)
    {
      handler(e, 0);
    }
    else
    {
      try
      {
        std::string archive_data(&inbound_data_[0], inbound_data_.size());
        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream);
        archive >> t;
      }
      catch (std::exception& e)
      {
        boost::system::error_code error(boost::asio::error::invalid_argument);
        handler(error, 0);
        return;
      }

      handler(e, 0);
    }
  }

  template <typename T>
  void read(T& t, boost::system::error_code& ec)
  {
    boost::asio::read(socket_, boost::asio::buffer(inbound_header_), ec);
    
    if(ec)
    {
      return;
    }
    else
    {
      std::istringstream is(std::string(inbound_header_, header_length));
      std::size_t inbound_data_size = 0;
      if (!(is >> std::hex >> inbound_data_size))
      {
        ec = boost::asio::error::invalid_argument;
        return;
      }

      inbound_data_.resize(inbound_data_size);
      boost::asio::read(socket_, boost::asio::buffer(inbound_data_), ec);

      if(ec)
      {
        return;
      }
      
      try
      {
        std::string archive_data(&inbound_data_[0], inbound_data_.size());
        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream);
        archive >> t;
      }
      catch (std::exception& e)
      {
        ec = boost::asio::error::invalid_argument;
        return;
      }

    }
  }

private:
  boost::asio::ip::tcp::socket socket_;

  enum { header_length = 8 };

  std::string outbound_header_;
  std::string outbound_data_;

  char inbound_header_[header_length];
  std::vector<char> inbound_data_;
};

using connection_ptr = boost::shared_ptr<connection>;

} // namespace networking

} // namespace gsdk

#endif // CONNECTION_HPP
