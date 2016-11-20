#include <iostream>
#include <unistd.h>
#include "Serial.h"
using namespace std;

void print_received(string str){
    cout << str << endl;
}

int main(){

    Serial *arduino = new Serial();
    arduino->Begin(9600,"/dev/ttyUSB0");
    arduino->set_Readcallback(print_received);

    cout << "Hello World" << std::endl;

    arduino->Read_ln();

    cout << "Comecei o sleep" << endl;

    usleep(5000*1000);

    arduino->Close();

    return 0;

}