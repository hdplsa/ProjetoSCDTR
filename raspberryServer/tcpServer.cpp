#include "tcpServer.h"

tcpServer::tcpServer(string ip, string port) : socket_(io), deadline(io)
{

  // Faz set do objeto que vai aceitar as ligações ao server
  acceptor = acceptor_ptr(new ip::tcp::acceptor(io, 
    ip::tcp::endpoint(ip::address::from_string(ip.c_str()), stoi(port))));
  
  // Set do timer sem nenhum tempo definido (ainda)
  deadline.async_wait(boost::bind(&tcpServer::check_deadline, this));

  working = true;
}

/*  Esta função trata de usar o resolver para obter os endereços dos sites
 * fornecidos (pode ou não ser util no projeto)
 */
void tcpServer::accept()
{

  cout << "A aceitar ligações." << endl;

  acceptor->async_accept(socket_,
    boost::bind(&tcpServer::handle_accept, this,
    boost::asio::placeholders::error));

  // Corre o resolve numa thread
  t = boost::thread(boost::bind(&boost::asio::io_service::run, &io));
}

void tcpServer::handle_accept(const boost::system::error_code &ec)
{

  // Se não tiver ocorrido erro, segue para ler o cliente
  if(!ec){

    cout << "Conexão aceite." << endl;

    start_read();
  } 
  
}

void tcpServer::start_read()
{
  // Set a deadline for the read operation.
  //deadline.expires_from_now(boost::posix_time::seconds(30));

  // Start an asynchronous operation to read a newline-delimited message.
  boost::asio::async_read_until(socket_, buf, '\n',
                                boost::bind(&tcpServer::handle_read, this, 
                                boost::asio::placeholders::error));

}

void tcpServer::handle_read(const boost::system::error_code &ec)
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

    if(onRead != NULL) onRead(line);

    start_read();
  }
  else
  {
    std::cout << "Error on receive: " << ec.message() << "\n";

    if(onReadError != NULL) onReadError();

    stop();
  }
}

void tcpServer::Write(string send)
{

  // Envia o send para o cliente tcp
  boost::asio::async_write(socket_, boost::asio::buffer(send, send.length()),
                           boost::bind(&tcpServer::handle_write, this, 
                           boost::asio::placeholders::error));
}

void tcpServer::handle_write(const boost::system::error_code &ec)
{

  if (!ec)
  {
    
    if(onWrite != NULL) onWrite();

  } else {
    cout << "Erro no envio" << endl;

    if(onWriteError != NULL) onWriteError();
  }
}

void tcpServer::check_deadline()
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
  deadline.async_wait(boost::bind(&tcpServer::check_deadline, this));
}

void tcpServer::set_Readcallback(std::function<void(string)> fcn){

  onRead = fcn;

}

void tcpServer::set_Writecallback(std::function<void(void)> fcn){

  onWrite = fcn;

}

void tcpServer::set_ReadErrorcallback(std::function<void(void)> fcn){

  onReadError = fcn;

}

void tcpServer::set_WriteErrorcallback(std::function<void(void)> fcn){

  onWriteError = fcn;

}

bool tcpServer::isWorking(){
  return working;
}

void tcpServer::stop()
{
  t.interrupt();
  socket_.close();
  deadline.cancel();
  working = false;
}

tcpServer::~tcpServer(){
  stop();
  io.stop();
}

