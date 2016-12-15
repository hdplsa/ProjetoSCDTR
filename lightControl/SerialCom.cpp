#include "SerialCom.h"

// Esta classe serve para gerir todas as transações que hajam por
// serial, controla os envios por serial, as receções, e o proces-
// samento das receções.

// Inicialização dos valores estáticos

int SerialCom::current_char = 0;
char SerialCom::incoming[10];
int SerialCom::occupancy = 1;
int SerialCom::currentLux = 0;
int SerialCom::ref = -1;
volatile bool SerialCom::new_o = false;
volatile bool SerialCom::new_ref = 0;

void SerialCom::Begin(long int Baudrate){

  Serial.begin(Baudrate);
  current_char = 0;
  
}

// Esta função recebe a string do computador e retorna-a 
void SerialCom::receive_message(){

  char in;

  while(Serial.available() > 0){
    in = Serial.read();
    // Cada mensagem enviada pelo pc tem que ser terminada por '\n'
    if(in != '\n'){
      
      // Se a mensagem ainda não tiver acabado, guarda o byte na string
      incoming[current_char] = in;
      current_char++; //Próximo byte fica na próxima posição
      
    } else {
      
      // Quando chegarmos ao final da string, temos que recomeçar o
      // contador de caracteres
      incoming[current_char] = '\0';
      current_char = 0;
      Serial.println(incoming);
      process_request(incoming);
      
    }
  }

  return;

}

// Esta função recebe a string do computador e decifra o seu comando
void SerialCom::process_request(char *message){

  int valor;
  char tipo;
  char text[10];
  
  sscanf(message,"%c %i",&tipo, &valor);

  switch(tipo){
    // Restard do Arduino (DESCOBRIR MÉTODO)
    case 'r':
        //Restart do ARDUINO <<<<<<<<--------------------------------------------------
      break;
    // Valor de ocupação do Arduino
    case 'o':
      new_o = true;
      occupancy = valor;
      break;
    // Caso em que queremos mudar a referência local
    case 'l':
      ref = valor;
      break;
    // Caso o tipo de comando seja desconhecido
    default:
      Serial.println((char*)"Wrong Command");
    break;
  } 
}

int SerialCom::getOccupancy(){
  new_o = false;
  return occupancy;
}

bool SerialCom::new_oc(){
  return new_o;
}

void SerialCom::set_currentLux(int currentLux){

  currentLux = currentLux;
  
}

int SerialCom::getRef(){

  int ref2 = ref;
  ref = -1;
  
  return ref2;
}

