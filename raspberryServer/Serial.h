#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

//#define	BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
using namespace std;
using namespace boost::asio;

typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;

class Serial {

    public:
        Serial();
        int Begin(long baudrate, const char* port);
        void set_Readcallback(std::function<void(string)>);
        void set_Writecallback(std::function<void(void)>);
        void set_ReadErrorcallback(std::function<void(void)>);
        void set_WriteErrorcallback(std::function<void(void)>);
        void Read_ln();
        void Write(std::string send);
        void Close();
        ~Serial();
        
    private:
        // Funções chamadas quando os processos assincronos finalizam
        void read_complete(const boost::system::error_code& e, std::size_t size);
        void write_complete(const boost::system::error_code& e, std::size_t size);

        // Callbacks
        std::function<void(string)> onRead = NULL;
        std::function<void(void)> onWrite = NULL;

        // Callbacks para o erro
        std::function<void(void)> onReadError = NULL;
        std::function<void(void)> onWriteError = NULL;

        // Serviço que realiza as transações do serial
        boost::asio::io_service io;
        boost::asio::io_service::work work;

        // Objeto da porta serial (é um smart pointer)
        boost::asio::serial_port arduino;

        // buffer para enviar e receber dados
        boost::asio::streambuf buf;

        // objeto usado para obter os erros do boost
        boost::system::error_code error;

        // Timer para garantir que não recebemos lixo
        boost::asio::deadline_timer deadline;

        // Thread para o serial ser relamente asincrono
        boost::thread t;

        // Valor boleano que nos diz se a thread está a ser usada ou não       
        bool working = false;

        enum { max_length = 1024 };
        char data_[max_length];

};

#endif