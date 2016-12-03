#include "tcpServer.h"

tcpServer::tcpServer(io_service& io_, string ip, string port) : 
  io(io_), 
  acceptor(io_, ip::tcp::endpoint(ip::address::from_string(ip.c_str()), stoi(port)))
  {

  cout << "A aceitar ligações em " << ip << ':' << port <<endl;

  accept();

  working = true;
}

/*  Esta função trata de usar o resolver para obter os endereços dos sites
 * fornecidos (pode ou não ser util no projeto)
 */
void tcpServer::accept()
{

  session* _session = new session(io);

  acceptor.async_accept(_session->get_socket(),
    boost::bind(&tcpServer::handle_accept, this,
    boost::asio::placeholders::error, _session));

  // Corre o resolve numa thread
  //t = boost::thread(boost::bind(&boost::asio::io_service::run, &io));
}

void tcpServer::handle_accept(const boost::system::error_code &ec, session* _session)
{

  // Se não tiver ocorrido erro, segue para ler o cliente
  if(!ec){

    cout << "Conexão aceite." << endl;

    std::function<void(string, session*)> fcn = std::bind(&tcpServer::handle_read, this, std::placeholders::_1, _session);

    _session->set_Readcallback(fcn);
    _session->start_read();
  
    sessions.push_front(_session);

  } else {

    cout << "Erro na criação da ligação: " << ec.message() << endl;

    delete _session;

  }

  accept();
  
}

//Recebe a mensagem das sessões e o callback para retornar o valor
void tcpServer::handle_read(string line, session* _session)
{

  // Ignora mensagens vazias
  if (!line.empty())
  {
    std::cout << "Received: " << line << "\n";
    Write(line, _session);
  }

  if(onRead != NULL) onRead(line);

}

void tcpServer::Write(string send, session* _session)
{

  // Envia o send para o cliente tcp
  boost::asio::async_write(_session->get_socket(), boost::asio::buffer(send, send.length()),
                           boost::bind(&session::handle_write, _session, 
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
  working = false;
}

tcpServer::~tcpServer(){
  stop();
  io.stop();
}

