#ifndef SERIALCOM_H_INCLUDED
#define SERIALCOM_H_INCLUDED

#include "Arduino.h"
#include "LumIntensity.h"

class SerialCom{

  LumIntensity *luminten;
  
  public:
    SerialCom(int Baudrate, LumIntensity *luminten);
    void send_message(char* message);
    char* receive_message();
    void process_request(char *message);
    
    
};


#endif
