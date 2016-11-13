#include <iostream>
#include "Serial.h"
using namespace std;

int main(){

    string port = "";

    Serial arduino = new Serial();
    arduino.begin(port, 115200);

    cout << arduino.read();

    cout << "Hello World";

    return 0;

}