#include "tcpServer.h"

tcpServer::tcpServer(){

  // Faz set do objeto que imprime no socket
  acceptor = acceptor_ptr(new ip::tcp::acceptor(
    io, ip::tcp::endpoint(ip::tcp::v4(), 10000)));
  // Faz set do objeto que recebe do socket
  resolver = resolver_ptr(new ip::tcp::resolver(io));

}

void tcpServer::connect(string ip, string port){

  ip::tcp::resolver::query query(ip, port);

  // Cria a tarefa de resolve
  resolver->async_resolve(query,
    boost::bind(&tcpServer::resolve_complete, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::iterator));
/*
  // Corre o resolve numa thread
  t = boost::thread(boost::bind(&boost::asio::io_service::run,&io)); */

}

void tcpServer::resolve_complete(const boost::system::error_code& e, ip::tcp::resolver::iterator endpoint_iterator){



}