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
                &Serial::read_complete,
                this, boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
        ));

        t = boost::thread(boost::bind(&boost::asio::io_service::run,&io));
        working = true; 

    } else {
        cout << "close" << endl;
    }					

}

void Serial::Write(std::string send){

    if(arduino->is_open()){

        // Escreve os dados no buffer
        ostream os(&buf);
        os.write(send.c_str(), send.length());

        async_write(*arduino, buf,
            boost::bind(
                &Serial::write_complete,
                this, boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
        ));

        t = boost::thread(boost::bind(&boost::asio::io_service::run,&io));
        working = true; 

    } else {
        cout << "Can't print interface is closed'" << endl;
    }	

}

void Serial::read_complete(const boost::system::error_code& e, std::size_t size){

    working = false;

    // Verifica se houve erro
    if(!e){
        istream is(&buf);
        string data(size,'\0');
        is.read(&data[0],size);

        std::cout << "Received data:" << data;
    }

}

void Serial::write_complete(const boost::system::error_code& e, std::size_t size){

    working = false;

    cout << "Imprimi" << endl;

}

void Serial::Close(){

    arduino->close();

}

Serial::~Serial(){

    arduino->close();

    // Não é preciso apagar o arduino, porque é um smart pointer

}