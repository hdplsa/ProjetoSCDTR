#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

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
        void start_read_ln();
        void Write(std::string send);
        void Close();
        ~Serial();
        
    private:
        void read_complete(const boost::system::error_code& e, std::size_t size);
        void write_complete(const boost::system::error_code& e, std::size_t size);
        boost::asio::io_service io;
        serial_port_ptr arduino;
        boost::asio::streambuf buf;
        boost::system::error_code error;

        // Thread para o serial ser relamente asincrono
        boost::thread t;

        // Valor boleano que nos diz se a thread está a funcionar ou não        
        bool working = false;

};


#endif