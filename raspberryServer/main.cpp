#include <iostream>
#include <unistd.h>
#include <string>
#include "Serial.h"
#include "tcpServer.h"
#include "Arduino.h"
#include <signal.h>

using namespace std;

const string host("127.0.0.1");
const string port("4444");

tcpServer *server = new tcpServer(host, port);

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

    // Diz o que deve acontecer quandp se carraga no CTRL+C
    signal(SIGINT, close_all); 

    server->accept();

    while(server->isWorking()){}

    delete server;

    return 0;

}