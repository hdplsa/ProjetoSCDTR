#include <unistd.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;


class client
{
public:
  client(boost::asio::io_service& io_service);

  // Called by the user of the client class to initiate the connection process.
  // The endpoint iterator will have been obtained using a tcp::resolver.
  void start(tcp::resolver::iterator endpoint_iter);

  // This function terminates all the actors to shut down the connection. It
  // may be called by the user of the client class, or by the class itself in
  // response to graceful termination or an unrecoverable error.
  void stop();
    void start_send(std::string send);

private:
  void start_connect(tcp::resolver::iterator endpoint_iter);

  void handle_connect(const boost::system::error_code& ec,
      tcp::resolver::iterator endpoint_iter);

  void start_read();

  void handle_read(const boost::system::error_code& ec);

  void start_write();

  void handle_write(const boost::system::error_code& ec);

  void check_deadline();

  
  void handle_read_console(const boost::system::error_code& ec, std::size_t length);
  void handle_send(const boost::system::error_code& ec, std::size_t length);
  

private:
  bool stopped_;
  tcp::socket socket_;
  boost::asio::streambuf input_buffer_;
  deadline_timer deadline_;
  deadline_timer heartbeat_timer_;
  boost::asio::posix::stream_descriptor input_;
  boost::asio::streambuf console_buffer_;
  enum {max_length=1024};
  char send_buffer_[max_length];
};