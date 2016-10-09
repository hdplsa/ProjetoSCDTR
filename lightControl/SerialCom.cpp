#include "SerialCom.h"

SerialCom::SerialCom(int Baudrate, LumItensity *luminten){

  Serial.begin(Baudrate);
  this->luminten = luminten;
  
}

// Esta função recebe uma string que depois envia para o computador
void SerialCom::send_message(char *message){

  Serial.println(message);
  
}

// Esta função recebe a string do computador e retorna-a 
char *SerialCom::receive_message(){

  char incoming[20];

  if(Serial.available() > 0){
    Serial.readBytesUntil('\0',incoming,20);
    return incoming;
  } else {
    return '\0';
  }

}

// Esta função recebe a string do computador e decifra o seu comando
void SerialCom::process_request(char *message){

  int tipo, valor;
  sscanf(message,"%i %i",&tipo, &valor);

  switch(tipo){
    // Caso em que queremos mudar a referência
    case 0:
      luminten->setLedLuminousItensity( (double) valor);
      break;
  }
  
}

