#ifndef SERIALCOM_H_INCLUDED
#define SERIALCOM_H_INCLUDED

#include "Arduino.h"

class SerialCom{

  // Variáveis usadas na receção de mensagens serial
  static int current_char;
  static char incoming[10]; //mensagens com 10 caractéres máximo, pode mudar.

  // Variáveis usadas na execução dos pedidos das mensagens
  static int occupancy;
  static int currentLux;
  static int ref;

  // Booleanos que dizem se há valores novos
  static volatile bool new_ref;
  
  public:
    static void Begin(long int Baudrate);
    static void receive_message();
    static void set_currentLux(int currentLux);
    static int getRef();
    static int getOccupancy();

  private:
    static void process_request(char *message);

};


#endif
