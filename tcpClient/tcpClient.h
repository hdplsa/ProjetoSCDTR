#ifndef TECSERVER_H_INCLUDED
#define tcpClient_H_INCLUDED

//#define	BOOST_ASIO_ENABLE_HANDLER_TRACKING //DEBUG MODE DO BOOST ASIO
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
using namespace std;
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::acceptor> acceptor_ptr;
typedef boost::shared_ptr<ip::tcp::resolver> resolver_ptr;
typedef boost::shared_ptr<ip::tcp::socket>   socket_ptr;
typedef boost::shared_ptr<deadline_timer>    deadline_ptr;

class tcpClient {

    public:
        tcpClient();
        void connect(string host, string ip);
        void Write(std::string send);
        bool isWorking();
        void stop();
        ~tcpClient();

        void set_Readcallback(void (*function)(string));
        void set_Writecallback(void (*function)(void));
        void set_ReadErrorcallback(void (*function)(void));
        void set_WriteErrorcallback(void (*function)(void));
        
    private:
        // Funções chamadas quando os processos assincronos finalizam
        void handle_resolve(const boost::system::error_code& e, ip::tcp::resolver::iterator endpoint_iterator);
        void handle_connect(const boost::system::error_code& ec, ip::tcp::resolver::iterator endpoint_iter);
        void start_read();
        void handle_read(const boost::system::error_code &ec);
        void start_heartbeat();
        void handle_heartbeat(const boost::system::error_code &ec);
        void handle_write(const boost::system::error_code &ec);
        void check_deadline();
        void write_complete(const boost::system::error_code& e, std::size_t size);

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

        // Objeto de fazer tcp requests
        resolver_ptr resolver;

        // Socket
        ip::tcp::socket socket_;

        // Timers para controlar os timeouts
        deadline_timer deadline;
        deadline_ptr heartbeat_timer;

        // buffer para enviar e receber dados
        boost::asio::streambuf buf;

        // objeto usado para obter os erros do boost
        boost::system::error_code error;

        // Thread para o os requests serem assincronos
        boost::thread t;

        // Valor boleano que nos diz se a thread está a ser usada ou não       
        bool working = false;

};

#endif