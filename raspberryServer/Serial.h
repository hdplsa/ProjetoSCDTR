#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED



#include <fcntl.h>

#include <unistd.h>
#include <cstring>

#include <termios.h>    // POSIX terminal control definitions
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace std;

class Serial {

int arduino;

public:
    Serial();
    void Begin(long baudrate, const char* port);
    void read_ln();
    void Write(string in);
    void Close();
    ~Serial();

    void set_Readcallback(std::function<void(char*)> fcn);
    void set_Writecallback(std::function<void(void)> fcn);
    
private:

    // Callbacks
    std::function<void(char*)> onRead = NULL;
    std::function<void(void)> onWrite = NULL;


};


#endif