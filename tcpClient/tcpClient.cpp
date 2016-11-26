#include "tcpClient.h"

tcpClient::tcpClient() : socket_(io), deadline(io)
{

  // Faz set do objeto que imprime no socket
  acceptor = acceptor_ptr(new ip::tcp::acceptor(
      io, ip::tcp::endpoint(ip::tcp::v4(), 10000)));
  // Faz set do objeto que recebe os IPs que damos ao socket
  resolver = resolver_ptr(new ip::tcp::resolver(io));

  working = true;

  deadline.async_wait(boost::bind(&tcpClient::check_deadline, this));
  heartbeat_timer = deadline_ptr(new deadline_timer(io));
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

/* Esta função recebe uma data de endereços de IP querecebemos do resolver
 * agora o objetivo é tentar ver se nos conseguimos ligar a um deles 
 *  Recebe:
 * e -> Codigo de erro do resolver
 * iterator -> objeto que tem os endereços de IP obtidos
 *  Chama:
 * Esta função chama uma rotina assincrona em que se liga o socket
 */
void tcpClient::handle_resolve(const boost::system::error_code &e, ip::tcp::resolver::iterator endpoint_iterator)
{

  if (endpoint_iterator != ip::tcp::resolver::iterator())
  {

    std::cout << "Trying " << endpoint_iterator->endpoint() << "...\n";

    // Dá um timeout para a ligação do socket
    deadline.expires_from_now(boost::posix_time::seconds(60));

    // Inicia a conexão ao host assincronamente
    socket_.async_connect(endpoint_iterator->endpoint(),
                          boost::bind(&tcpClient::handle_connect,
                                      this, _1, endpoint_iterator));
  }
}

/* Esta função é chamada quando o socket se liga (ou não) a um dos IPs
 * tem que conseguir voltar atrás ao handle_resolve, caso não se consiga ligar,
 * para o handle_resolve tentar ligar a outro IP
 *  Recebe:
 * ec -> código de erro 
 * iterator -> objeto com os vários endereços de IP obtidos do resolver
 *  Chama:
 * handle_resolve -> caso haja erro, tenta o próximo IP
 * start_read e start_heartbeat -> caso s etenha ligado bem
 */
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
    start_heartbeat();

  }
}

/* Função que inicia um processo de leitura.
 *  Chama:
 * async_read_until -> função que vai ler os dados do socket
 * handle_read -> após o read ser acabado
 */
void tcpClient::start_read()
{

  // Dá um tempo máximo para a operação de read
  deadline.expires_from_now(boost::posix_time::seconds(30));

  // Começa uma leitura assincrona que acaba sempre que recebe o '\n'
  boost::asio::async_read_until(socket_, buf, '\n',
                                boost::bind(&tcpClient::handle_read, this, _1));

}
/* Função que processa a leitura de uma string do socket.
 *  Chama:
 * start_read -> recomeça um processo de leitura mal este acabe
 */
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

    stop();
  }
}

/* Função que envia um caracter '\n' para simbolizar ao servidor que a ligação
 * ainda está a ser utilizada.
 *  Chama:
 * async_write -> função que manda '\n' para o servidor
 */
void tcpClient::start_heartbeat()
{

  // Start an asynchronous operation to send a heartbeat message.
  boost::asio::async_write(socket_, boost::asio::buffer("\n", 1),
                           boost::bind(&tcpClient::handle_heartbeat, this, _1));
}

/* Função que é chamada depois do heartbeat ser enviado. É enviado um novo
 * heartbeat a cada 10 segundos.
 * ainda está a ser utilizada.
 *  Chama:
 * start_heartbeat -> função que envia o hearbeat
 */
void tcpClient::handle_heartbeat(const boost::system::error_code &ec)
{

  if (!ec)
  {
    // Wait 10 seconds before sending the next heartbeat.
    heartbeat_timer->expires_from_now(boost::posix_time::seconds(10));
    heartbeat_timer->async_wait(boost::bind(&tcpClient::start_heartbeat, this));
  }
  else
  {
    std::cout << "Error on heartbeat: " << ec.message() << "\n";

    stop();
  }
}

/* Manda para o server a mensagem send.
 * ainda está a ser utilizada.
 *  Chama:
 * async_write -> função que envia a mensagem assincronamente
 */
void tcpClient::Write(std::string send){

  // Start an asynchronous operation to send a heartbeat message.
  boost::asio::async_write(socket_, boost::asio::buffer(send, send.length()),
                           boost::bind(&tcpClient::handle_write, this, _1));

}

void tcpClient::handle_write(const boost::system::error_code &ec){

  if(ec){

    cout << "Aconteceu um erro a enviar a mensagem, tente novamente." << endl;

  }

}

void tcpClient::check_deadline()
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
  deadline.async_wait(boost::bind(&tcpClient::check_deadline, this));
}

bool tcpClient::isWorking(){
  return working;
}

void tcpClient::stop()
{
  socket_.close();
  deadline.cancel();
  heartbeat_timer->cancel();
  working = false;
}

tcpClient::~tcpClient(){
  stop();
}
