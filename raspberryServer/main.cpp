#include <iostream>
#include <string>
#include "Serial.h"
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

    Arduino *arduino = new Arduino(1000, "/dev/ttyUSB0");

    arduino->ledON();

    usleep(5000*1000);

    arduino->ledOFF();

    usleep(1000*1000);

}

int main(){

    const string host("127.0.0.1");
    const string port("4444");

    tcpServer *server = new tcpServer(host, port);
    server->accept();

    // Inicia o mailController
    	// Portas dos arduinos
	const string port1 = "/dev/ttyUSB0";
	const string port2 = "/dev/ttyUSB1";

    vector<string> ports = {port1};

    MainController controller(1, ports);

    server->set_Readcallback(std::bind(&MainController::get_clientRequest, &controller, std::placeholders::_1, std::placeholders::_2));

    // Diz o que deve acontecer quandp se carraga no CTRL+C
    //signal(SIGINT, close_all); 

    server->accept();

    while(server->isWorking()){}

    delete server;

    return 0;

}