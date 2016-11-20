#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
using namespace std;
using namespace boost::asio;

typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;

class Serial {

    public:
        Serial();
        int Begin(long baudrate, const char* port);
        void start_read_ln();
        void Close();
        ~Serial();
        
    private:
        void process_read(const boost::system::error_code& e, std::size_t size);
        boost::asio::io_service io;
        serial_port_ptr arduino;
        boost::asio::streambuf buf;
        boost::system::error_code error;
        int count = 0;

};


#endif