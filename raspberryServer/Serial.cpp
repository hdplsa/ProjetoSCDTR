#include "Serial.h"

Serial::Serial(){

    arduino = serial_port_ptr(new boost::asio::serial_port(io));

}

int Serial::Begin(long baudrate, const char* port){

    
    arduino->open(port, error); //connect to port

    // Error handling
    if(error){
        cout << "Error";
        return -1;
    }

    //set baud rate
    arduino->set_option(boost::asio::serial_port_base::baud_rate(baudrate),error);
    if(error){
        cout <<	"Error";	
        return -1;
    }		

    return 0;

}

void Serial::start_read_ln(){

	if(arduino->is_open()){

        async_read_until(*arduino, buf, '\n',
            boost::bind(
                &Serial::process_read,
                this, boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
        ));

        io.run();   

    } else {
        cout << "close" << endl;
    }					

}

void Serial::process_read(const boost::system::error_code& e, std::size_t size){

    // Verifica se houve erro
    if(!e){
        istream is(&buf);
        string data(size,'\0');
        is.read(&data[0],size);

        std::cout << "Received data:" << data;
    }

}

void Serial::Close(){

    arduino->close();

}

Serial::~Serial(){


}