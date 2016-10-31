#include "SerialCom.h"

// Esta classe serve para gerir todas as transações que hajam por
// serial, controla os envios por serial, as receções, e o proces-
// samento das receções.

SerialCom::SerialCom(long int Baudrate){

  Serial.begin(Baudrate);
  this->current_char = 0;
  
}

// Esta função recebe uma string que depois envia para o computador
void SerialCom::send_message(char *message){

  Serial.println(message);
  
}

// Esta função recebe a string do computador e retorna-a 
void SerialCom::receive_message(){

  char in;

  while(Serial.available() > 0){
    in = Serial.read();
    // Cada mensagem enviada pelo pc tem que ser terminada por '\n'
    if(in != '\n'){
      
      // Se a mensagem ainda não tiver acabado, guarda o byte na string
      this->incoming[current_char] = in;
      current_char++; //Próximo byte fica na próxima posição
      
    } else {
      
      // Quando chegarmos ao final da string, temos que recomeçar o
      // contador de caracteres
      this->incoming[current_char] = '\0';
      current_char = 0;
      this->send_message(this->incoming);
      this->process_request(this->incoming);
      
    }
  }

  return;

}

// Esta função recebe a string do computador e decifra o seu comando
void SerialCom::process_request(char *message){

  int tipo, valor;
  char text[10];
  
  sscanf(message,"%i %i",&tipo, &valor);

  switch(tipo){
    // Muda o valor do LED PWM
    case 0:
      this->valorLed = valor;
      break;
    // Envia os valores do LDR em lux
    case 1:
      sprintf(text, "%i", currentLux);
      this->send_message(text);
      break;
    // Caso em que queremos mudar a referência local
    case 2:
      this->ref = valor;
      break;
    // Significado da vida, a board fica tão à toa que faz reboot
    case 42:
      // faz reset
      break;
    // Caso o tipo de comando seja desconhecido
    default:
      this->send_message((char*)"Wrong Command");
    break;
  }
  
}

int SerialCom::get_valorLed(){

  return this->valorLed;
  
}

void SerialCom::set_currentLux(int currentLux){

  this->currentLux = currentLux;
  
}

int SerialCom::get_ref(){

  return this->ref;
  
}

