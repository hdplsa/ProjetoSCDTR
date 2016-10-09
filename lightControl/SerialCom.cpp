#include "SerialCom.h"

// Esta classe serve para gerir todas as transações que hajam por
// serial, controla os envios por serial, as receções, e o proces-
// samento das receções.

SerialCom::SerialCom(int Baudrate, LumIntensity *luminten){

  Serial.begin(Baudrate);
  this->luminten = luminten;
  this->current_char = 0;
  
}

// Esta função recebe uma string que depois envia para o computador
void SerialCom::send_message(char *message){

  Serial.println(message);
  
}

// Esta função recebe a string do computador e retorna-a 
void SerialCom::receive_message(){

  char in;

  if(Serial.available() > 0){
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
  } else {
    return;
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
    // Caso o tipo de comando seja desconhecido
    default:
      this->send_message((char*)"Wrong Commang");
    break;
  }
  
}

