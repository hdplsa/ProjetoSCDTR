#include <iostream>
#include <unistd.h>
#include "Arduino.h"
using namespace std;

void print_received(string str)
{
    cout << str << endl;
}

int main()
{

    Arduino *arduino = new Arduino(1000, "/dev/ttyACM0");

    arduino->ledON();

    usleep(5000*1000);

    arduino->ledOFF();

    usleep(1000*1000);

    return 0;

}