#ifndef SERIALCOM_H_INCLUDED
#define SERIALCOM_H_INCLUDED

#include "Arduino.h"
#include "LumIntensity.h"

class SerialCom{

  LumIntensity *luminten;
  int current_char;
  char incoming[10];
  
  public:
    SerialCom(int Baudrate, LumIntensity *luminten);
    void send_message(char* message);
    void receive_message();

  private:
    void process_request(char *message);
    
    
};


#endif
