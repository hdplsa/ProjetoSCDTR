#include <iostream>
#include "Serial.h"
using namespace std;

int main(){

    Serial *arduino = new Serial();
    arduino->Begin(115200,"/dev/ttyACM0");

    cout << "Hello World" << std::endl;

    for(int i = 0; i < 100; i++){
        arduino->read_ln();
    }

    for(int i = 0; i < 5; i++){
        arduino->Write("Whore");
    }
    
    for(int i = 0; i < 100; i++){
        arduino->read_ln();
    }

    arduino->Close();

    return 0;

}