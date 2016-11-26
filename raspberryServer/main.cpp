#include <iostream>
#include <unistd.h>
#include <string>
#include "Serial.h"
#include "tcpServer.h"
using namespace std;

void print_received(string str){
    cout << str << endl;
}

void test_serial(){

    Serial *arduino = new Serial();
    arduino->Begin(9600,"/dev/ttyUSB0");
    arduino->set_Readcallback(print_received);

    cout << "Hello World" << std::endl;

    arduino->Read_ln();

    cout << "Comecei o sleep" << endl;

    usleep(5000*1000);

    arduino->Close();

}

int main(){

    tcpServer *server = new tcpServer();

    string host("127.0.0.1");
    string port("4444");

    server->connect(host,port);

    usleep(5000*1000);


    return 0;

}