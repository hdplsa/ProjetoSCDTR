#include <iostream>
#include <unistd.h>
#include <string>
#include "Serial.h"
#include "tcpServer.h"
#include "Arduino.h"
#include <signal.h>
#include <boost/asio.hpp>

using namespace std;

tcpServer *server;

// Função chamada quando se dá CTRL + C 
void close_all(int sig){

    server->stop();

}

void print_received(string str)
{
    cout << str << endl;
}

void test_serial(){

    Arduino *arduino = new Arduino(1000, "/dev/ttyACM0");

    arduino->ledON();

    usleep(5000*1000);

    arduino->ledOFF();

    usleep(1000*1000);

}

int main(){

    // Diz o que deve acontecer quando se carraga no CTRL+C
    signal(SIGINT, close_all); 

    try{
        // Serviço que realiza as transações do servidor
        boost::asio::io_service server_io;

        const string host("127.0.0.1");
        const string port("4444");

        server = new tcpServer(server_io,host, port);

        server_io.run();

    }catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }


    return 0;

}