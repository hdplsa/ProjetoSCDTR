#ifndef TECSERVER_H_INCLUDED
#define tcpClient_H_INCLUDED

#define	BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
using namespace std;
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::acceptor> acceptor_ptr;
typedef boost::shared_ptr<ip::tcp::resolver> resolver_ptr;
typedef boost::shared_ptr<ip::tcp::socket>   socket_ptr;

class tcpServer {

    public:
        tcpServer(string ip, string port);
        void accept();
        void Write(std::string send);
        bool isWorking();
        void stop();
        ~tcpServer();

        void set_Readcallback(void (*function)(string));
        void set_Writecallback(void (*function)(void));
        void set_ReadErrorcallback(void (*function)(void));
        void set_WriteErrorcallback(void (*function)(void));
        
    private:
        // Funções chamadas quando os processos assincronos finalizam
        void handle_accept(const boost::system::error_code& e);
        void start_read();
        void handle_read(const boost::system::error_code &ec);
        void handle_write(const boost::system::error_code &ec);
        void check_deadline();
    
        // Callbacks
        void (*onRead)(string) = NULL;
        void (*onWrite)(void) = NULL;

        // Callbacks para o erro
        void (*onReadError)(void) = NULL;
        void (*onWriteError)(void) = NULL;

        // Serviço que realiza as transações do servidor
        boost::asio::io_service io;

        // Objeto de receber tcp requests
        acceptor_ptr acceptor;

        // Socket
        ip::tcp::socket socket_;

        // Timers para controlar os timeouts
        deadline_timer deadline;

        // buffer para enviar e receber dados
        boost::asio::streambuf buf;

        // Thread para o os requests serem assincronos
        boost::thread t;

        // Valor boleano que nos diz se o server está a ser usado ou não      
        bool working = false;

};

#endif