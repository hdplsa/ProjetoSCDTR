#include <iostream>
#include <unistd.h>
#include <string>
#include <signal.h>
#include "tcpClient.h"
using namespace std;

tcpClient *client = new tcpClient();

void close_all(int sig){

    delete client;

}

int main(){

    // Diz o que deve acontecer quandp se carraga no CTRL+C
    signal(SIGINT, close_all); 

    string host("127.0.0.1");
    string port("4444");

    client->connect(host,port);

    cout << "Insira o comando a mandar ao raspberry pi:" << endl;

    string str;

    while(client->isWorking()){

        cin >> str;

        if(str == "quit") {close_all(0); break;}

        if(str.c_str()[0] != '\n'){
            client->Write(str);
        }

    };

    return 0;

}