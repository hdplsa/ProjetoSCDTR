#ifndef SERIALCOM_H_INCLUDED
#define SERIALCOM_H_INCLUDED

#include "Arduino.h"
#include "LumItensity.h"

class SerialCom{

  LumItensity *luminten;
  
  public:
    SerialCom(int Baudrate, LumItensity *luminten);
    void send_message(char* message);
    char* receive_message();
    void process_request(char *message);
    
    
};


#endif
