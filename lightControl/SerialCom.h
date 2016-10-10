#ifndef SERIALCOM_H_INCLUDED
#define SERIALCOM_H_INCLUDED

#include "Arduino.h"

class SerialCom{

  // Variáveis usadas na receção de mensagens serial
  int current_char;
  char incoming[10];

  // Variáveis usadas no na execução dos pedidos das mensagens
  int valorLed;
  int currentLux;
  int ref;
  
  public:
    SerialCom(int Baudrate);
    void send_message(char* message);
    void receive_message();
    int get_valorLed();
    void set_currentLux(int currentLux);
    int get_ref();

  private:
    void process_request(char *message);
    
    
};


#endif
