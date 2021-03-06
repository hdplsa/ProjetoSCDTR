#ifndef TCPSERVER_H_INCLUDED
#define tcpClient_H_INCLUDED

#define	BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "session.h"
using namespace std;
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::acceptor> acceptor_ptr;
typedef boost::shared_ptr<ip::tcp::resolver> resolver_ptr;
typedef boost::shared_ptr<ip::tcp::socket>   socket_ptr;

class tcpServer {

    public:
        tcpServer(string ip, string port);
        void accept();
        void Write(string send, session* _session);
        bool isWorking();
        void stop();
        ~tcpServer();

        void set_Readcallback(std::function<void(string,std::function<void(string)>,void*)> fcn);
        void set_Writecallback(std::function<void(void)> fcn);
        void set_ReadErrorcallback(std::function<void(void)> fcn);
        void set_WriteErrorcallback(std::function<void(void)> fcn);
        void set_SessionDeletecallback(std::function<void(void*)> fcn);

        void handle_read(string line, session* _session);
        void handle_write();
        void delete_session(session* _session);
        
    private:
        // Funções chamadas quando os processos assincronos finalizam
        void handle_accept(const boost::system::error_code &ec, session* _session);
        void start_read();
        void check_deadline();
    
        // Callbacks
        std::function<void(string,std::function<void(string)>,void*)> onRead = NULL;
        std::function<void(void)> onWrite = NULL;

        // Callbacks para o erro
        std::function<void(void)> onReadError = NULL;
        std::function<void(void)> onWriteError = NULL;
        std::function<void(void*)> onSessionDelete = NULL;

        // Serviço que realiza as transações do servidor
        boost::asio::io_service io;

        // Objeto de receber tcp requests
        acceptor_ptr acceptor;

        // Timers para controlar os timeouts
        deadline_timer deadline;

        // buffer para enviar e receber dados
        boost::asio::streambuf buf;

        // Thread para o os requests serem assincronos
        boost::thread t;

        // Valor boleano que nos diz se o server está a ser usado ou não      
        bool working = false;

        // Lista de sessions para depois ser facil terminar todas
        list<session*> sessions;

};

#endif