#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <termios.h>    // POSIX terminal control definitions

using namespace std;

class Serial {

int arduino;

public:
    Serial();
    void Begin(long baudrate, const char* port);
    string read_ln();
    void Write(string in);
    void Close();
    ~Serial();
    
private:

};


#endif