#include <iostream>
#include <string>
#include "tcpServer.h"
#include "mainController.h"
#include "Arduino.h"

using namespace std;

// Função chamada quando se dá CTRL + C 
//void close_all(int sig){
//
//    server->stop();
//
//}

void print_received(string str)
{
    cout << str << endl;
}

void test_serial(){

    Arduino *arduino = new Arduino(1000, "/dev/ttyUSB1");

    arduino->ledON();

    usleep(5000*1000);

    arduino->ledOFF();

    usleep(1000*1000);

}

int main(){

        // Inicia o mailController
    	// Portas dos arduinos
	const string port1 = "/dev/ttyACM0";
	const string port2 = "/dev/ttyACM1";

    vector<string> ports = {port1,port2};

    MainController controller(2, ports);

    const string host("127.0.0.1");
    const string port("4444");

    tcpServer *server = new tcpServer(host, port);
    server->accept();

    server->set_Readcallback(std::bind(&MainController::get_clientRequest, &controller, std::placeholders::_1, std::placeholders::_2));

    // Diz o que deve acontecer quandp se carraga no CTRL+C
    //signal(SIGINT, close_all); 

    server->accept();

    while(server->isWorking()){}

    delete server;

    return 0;

}