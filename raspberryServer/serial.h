#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <termios.h>    // POSIX terminal control definitions

using namespace std;

class Serial {

int arduino;

public:
    Serial();
    void Begin(long baudrate, const string port);
    string read();
    void write();
    
private:

}


#endif