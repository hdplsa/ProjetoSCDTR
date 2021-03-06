#include "Serial.h"

Serial::Serial(){



}

void Serial::Begin(long baudrate, const char* port){

    arduino = open(port, O_RDWR| O_NOCTTY );

    if(arduino == -1){
        throw std::runtime_error("Erro a criar a porta Serial.");
    }

    struct termios tty;
    std::memset (&tty, 0, sizeof tty);

    /* Error Handling */
    if ( tcgetattr ( arduino, &tty ) != 0 ) {
        std::cout << "Error " << " from tcgetattr: " << std::endl;
    }

    /* Set Baud Rate */
    cfsetospeed (&tty, (speed_t)B115200);
    cfsetispeed (&tty, (speed_t)B115200);

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    /* Make raw */
    cfmakeraw(&tty);

    /* Flush Port, then applies attributes */
    tcflush( arduino, TCIFLUSH );
    if ( tcsetattr ( arduino, TCSANOW, &tty ) != 0) {
        std::cout << "Error" << " from tcsetattr" << std::endl;
    }

}

void Serial::Write(string str){

    if(str[str.length()-1] != '\n'){
        str += '\n';
        cout << str[str.length()];
    }

    for(unsigned int i = 0; i < str.length(); i++){
        write(arduino, &str[i], 1);
    }

}

void Serial::read_ln(){

    int n = 0, i = 0;
    char *str;
    char buffer = '\0';
    str = new char[100];
    while(1){
        n = 0; i = 0;
        do {
            n = read(arduino, &buffer, 1);
            str[i] = buffer;
            i++;
        } while(buffer != '\n' && n > 0 && n < 10);
        str[i] = '\0';   

        boost::this_thread::interruption_point();

        if(onRead != NULL) onRead(str);
    }

    delete str;

}

void Serial::set_Readcallback(std::function<void(char*)> fcn){
    onRead = fcn;
}

void Serial::set_Writecallback(std::function<void(void)> fcn){
    onWrite = fcn;
}

void Serial::Close(){

    close(arduino);
    cout << "Fechei" << endl;
    usleep(5000);

}

Serial::~Serial(){

    Close();
}