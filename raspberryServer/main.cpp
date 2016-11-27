#include <iostream>
#include <unistd.h>
#include <string>
#include "Serial.h"
#include "tcpServer.h"
#include "Arduino.h"

using namespace std;

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

    string host("127.0.0.1");
    string port("4444");

    tcpServer *server = new tcpServer(host, port);

    server->accept();

    while(server->isWorking()){}

    return 0;

}