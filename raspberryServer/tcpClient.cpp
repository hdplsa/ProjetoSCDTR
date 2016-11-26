#include "tcpClient.h"

tcpClient::tcpClient() : socket_(io)
{

  // Faz set do objeto que imprime no socket
  acceptor = acceptor_ptr(new ip::tcp::acceptor(
      io, ip::tcp::endpoint(ip::tcp::v4(), 10000)));
  // Faz set do objeto que recebe os IPs que damos ao socket
  resolver = resolver_ptr(new ip::tcp::resolver(io));

  // Inicia o timer
  deadline = deadline_ptr(new deadline_timer(io));
  // Set do timer sem nenhum tempo definido (ainda)
  deadline->async_wait(boost::bind(&tcpClient::check_deadline, this));
}

/*  Esta função trata de usar o resolver para obter os endereços dos sites
 * fornecidos (pode ou não ser util no projeto)
 */
void tcpClient::connect(string ip, string port)
{

  ip::tcp::resolver::query query(ip.c_str(), port.c_str());

  // Cria a tarefa de vai ao ip ou site que fornecemos e
  // nos devolve os endereços desse site para o handle_resolve
  resolver->async_resolve(query,
                          boost::bind(&tcpClient::handle_resolve, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::iterator));

  // Corre o resolve numa thread
  t = boost::thread(boost::bind(&boost::asio::io_service::run, &io));
}

void tcpClient::handle_resolve(const boost::system::error_code &e, ip::tcp::resolver::iterator endpoint_iterator)
{
  cout << "cheguei ao handle_resolve" << endl;
  if (endpoint_iterator != ip::tcp::resolver::iterator())
  {

    std::cout << "Trying " << endpoint_iterator->endpoint() << "...\n";

    // Dá um timeout para a ligação do socket
    deadline->expires_from_now(boost::posix_time::seconds(60));

    // Inicia a conexão ao host assincronamente
    socket_.async_connect(endpoint_iterator->endpoint(),
                          boost::bind(&tcpClient::handle_connect,
                                      this, _1, endpoint_iterator));
  }
}

void tcpClient::handle_connect(const boost::system::error_code &ec, ip::tcp::resolver::iterator endpoint_iterator)
{

  // The async_connect() function automatically opens the socket at the start
  // of the asynchronous operation. If the socket is closed at this time then
  // the timeout handler must have run first.
  if (!socket_.is_open())
  {
    std::cout << "Connect timed out\n";

    // Try the next available endpoint.
    boost::system::error_code e;
    handle_resolve(e, ++endpoint_iterator);
  }

  // Check if the connect operation failed before the deadline expired.
  else if (ec)
  {
    std::cout << "Connect error: " << ec.message() << "\n";

    // We need to close the socket used in the previous connection attempt
    // before starting a new one.
    socket_.close();

    // Try the next available endpoint.
    boost::system::error_code e;
    handle_resolve(e, ++endpoint_iterator);
  }

  // Otherwise we have successfully established a connection.
  else
  {
    std::cout << "Connected to " << endpoint_iterator->endpoint() << "\n";

    // Start the input actor.
    start_read();

    // Start the heartbeat actor.
    start_write();
  }
}

void tcpClient::start_read()
{
  // Set a deadline for the read operation.
  deadline->expires_from_now(boost::posix_time::seconds(30));

  // Start an asynchronous operation to read a newline-delimited message.
  boost::asio::async_read_until(socket_, buf, '\n',
                                boost::bind(&tcpClient::handle_read, this, _1));
}

void tcpClient::handle_read(const boost::system::error_code &ec)
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

    start_read();
  }
  else
  {
    std::cout << "Error on receive: " << ec.message() << "\n";

    //stop();
  }
}

void tcpClient::start_write()
{

  // Start an asynchronous operation to send a heartbeat message.
  boost::asio::async_write(socket_, boost::asio::buffer("\n", 1),
                           boost::bind(&tcpClient::handle_write, this, _1));
}

void tcpClient::handle_write(const boost::system::error_code &ec)
{

  if (!ec)
  {
    // Wait 10 seconds before sending the next heartbeat.
    heartbeat_timer->expires_from_now(boost::posix_time::seconds(10));
    heartbeat_timer->async_wait(boost::bind(&tcpClient::start_write, this));
  }
  else
  {
    std::cout << "Error on heartbeat: " << ec.message() << "\n";

    //stop();
  }
}

void tcpClient::check_deadline()
{

  // Check whether the deadline has passed. We compare the deadline against
  // the current time since a new asynchronous operation may have moved the
  // deadline before this actor had a chance to run.
  if (deadline->expires_at() <= deadline_timer::traits_type::now())
  {
    // The deadline has passed. The socket is closed so that any outstanding
    // asynchronous operations are cancelled.
    socket_.close();

    // There is no longer an active deadline. The expiry is set to positive
    // infinity so that the actor takes no action until a new deadline is set.
    deadline->expires_at(boost::posix_time::pos_infin);
  }

  // Put the actor back to sleep.
  deadline->async_wait(boost::bind(&tcpClient::check_deadline, this));
}
