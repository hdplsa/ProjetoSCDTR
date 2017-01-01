#include "session.h"

session::session(boost::asio::io_service& io) : socket_(io), deadline(io){

    // Set do timer sem nenhum tempo definido (ainda)
    deadline.async_wait(boost::bind(&session::check_deadline, this));

}

ip::tcp::socket& session::get_socket(){

    return this->socket_;

}

void session::start_read()
{
  // Set a deadline for the read operation.
  deadline.expires_from_now(boost::posix_time::seconds(30));

  // Start an asynchronous operation to read a newline-delimited message.
  boost::asio::async_read_until(socket_, buf, '\n',
                                boost::bind(&session::handle_read, this, 
                                boost::asio::placeholders::error));

}

void session::handle_read(const boost::system::error_code &ec)
{

  if (!ec)
  {
    // Extract the newline-delimited message from the buffer.
    std::string line;
    std::istream is(&buf);
    std::getline(is, line);

    // Empty messages are heartbeats and so ignored.
    if (!line.empty())
    {
      std::cout << "Received: " << line << "\n";
    }

    if(onRead != NULL) onRead(line, this);

    start_read();
  }
  else
  {
    std::cout << "Error on receive: " << ec.message() << "\n";

    if(onReadError != NULL) onReadError();

    //stop();
  }
}

void session::Write(string send)
{

  boost::unique_lock<boost::mutex> lock(mut);
  while(sending) cv.wait(lock);
  sending = true;

    // Set a deadline for the read operation.
  deadline.expires_from_now(boost::posix_time::seconds(30));

  // Envia o send para o cliente tcp
  boost::asio::async_write(socket_, boost::asio::buffer(send, send.length()),
                           boost::bind(&session::handle_write, this, 
                           boost::asio::placeholders::error));
}

void session::handle_write(const boost::system::error_code &ec)
{

  // Abre o mutex, dizendo à thread seguinte que pode avançar
  boost::lock_guard<boost::mutex> lock(mut);
  sending = false;
  cv.notify_one();

  if (!ec)
  {
    
    if(onWrite != NULL) onWrite();

  } else {
    cout << "Erro no envio" << endl;

    if(onWriteError != NULL) onWriteError();
  }
}

void session::check_deadline()
{

  // Check whether the deadline has passed. We compare the deadline against
  // the current time since a new asynchronous operation may have moved the
  // deadline before this actor had a chance to run.
  if (deadline.expires_at() <= deadline_timer::traits_type::now())
  {
    // The deadline has passed. The socket is closed so that any outstanding
    // asynchronous operations are cancelled.
    socket_.close();

    // There is no longer an active deadline. The expiry is set to positive
    // infinity so that the actor takes no action until a new deadline is set.
    deadline.expires_at(boost::posix_time::pos_infin);
  }

  // Put the actor back to sleep.
  deadline.async_wait(boost::bind(&session::check_deadline, this));
}

void session::set_Readcallback(std::function<void(string, session*)> fcn){

  onRead = fcn;

}

void session::set_Writecallback(std::function<void(void)> fcn){

  onWrite = fcn;

}

void session::set_ReadErrorcallback(std::function<void(void)> fcn){

  onReadError = fcn;

}

void session::set_WriteErrorcallback(std::function<void(void)> fcn){

  onWriteError = fcn;

}

session::~session(){
    socket_.close();
    deadline.cancel();
}