#include <iostream>
#include <unistd.h>
#include "Serial.h"
using namespace std;

int main(){

    Serial *arduino = new Serial();
    arduino->Begin(9600,"/dev/ttyUSB0");

    cout << "Hello World" << std::endl;

    arduino->start_read_ln();

    usleep(5000*1000);

    arduino->Close();

    return 0;

}