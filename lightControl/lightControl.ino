#include <avr/io.h>
#include <avr/interrupt.h>
#include "SerialCom.h"
#include "LightController.h"
#include "MetaClass.h"
#include "EEPROM.h"
#include "TWI.h"

// Objetos a serem utilizados
Meta *meta;
LightController * controller;

// Numeros das portas usadas
constexpr int ledPin = 11;
constexpr int sensorPin = 1;

constexpr uint8_t deviceID = 2;
volatile int Narduinos = 0;

void initTimer1(){
  cli();
  TCCR1A = 0;// clear register
  TCCR1B = 0;// clear register
  TCNT1 = 0;//reset counter
  OCR1A = 313; //must be <65536
  // = (16*10^6) / (1*1024) – 1
  TCCR1B |= (1 << WGM12); //CTC On
  // Set prescaler for 1024
  TCCR1B |= (1<<CS12)|(1<<CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); //allow interrupts
}

/*Funcao callback, chamada sempre que
 * e recebida uma string via I2C, e 
 * que copia a string recebida para 
 * a class Meta
 */
void metaI2CString(char *str){
  Serial.println(str);
  meta->receivedI2C(str);
}

/*Funcao callback, chamada sempre que
 * e enviada uma string via I2C, e 
 * que modifica a flag da class Meta
 */
void sendI2CState(){
  meta->setSendFlag(true);
}

volatile bool send_success = false;
volatile bool send_error   = false;

void count_I2Csend(){
  send_success = true;
}

void count_I2CsendError(){
  send_error = true;
}

int count_TWI(){

  char msg[] = "\n";

  int count = 1;

  // Itera sobre todos os endereços
  for(int i = 11; i <= 120; i++){

    Serial.print(i);
    Serial.print('\n');
    // Envia 
    TWI::send_msg(i, msg, 1);
    
    // Espera até o TWI conseguir ou não mandar para esse endereço
    while(send_success == false && send_error == false){}

    // Se conseguirmos mandar, então existe esse endereço
    if(send_success){
      count++;
      send_success = false;
    }
    // Se não onseguirmos mandar, então não há esse endereço
    if(send_error){
      send_error = false;
    } 
  }
  return count;
}

void get_Narduinos(char* str){
  if(str[0] == 'C'){
    Narduinos = str[2];
  }
}

void countArduinos(){
  // O arduino 10 faz a contagem do numero de arduinos
  if(EEPROM.read(0) == 10){
      //O MASTER deve esperar pelos restantes
      delay(2000);
      //Funções callback durante a contagem
      TWI::onSend(count_I2Csend);
      TWI::onSendError(count_I2CsendError);
      Narduinos = count_TWI();
      //Envio de numero de arduinos para os restantes
      char msg[4] = {'C', ' ', (char)Narduinos, '\0'};
      TWI::send_msg(0,msg,3);
      // Espera que a mensagem seja enviada
      while(send_success == false && send_error == false){}

      // Remove o onSendError que não é mais usado
      TWI::onSendError(NULL);
      
  } else { // Os restantes arduinos têm que receber o Narduinos por TWI

    TWI::onReceive(get_Narduinos);
    // Espera para receber o Narduino
    while(!Narduinos){}
  }
}

void setup() {
  // put your setup code here, to run once:
  
  SerialCom::Begin(115200);

    //Inicialização do I2C
  TWI::begin(EEPROM.read(0));
  //EEPROM.write(0,11);
  Serial.println(EEPROM.read(0));

  //Conta o numero de arduinos
  countArduinos();
  Serial.print("Narduinos: ");
  Serial.println(Narduinos);

  //Define funções callback
  TWI::onReceive(metaI2CString);
  TWI::onSend(sendI2CState);

  //Inicializações do controlador
  meta = new Meta(Narduinos,ledPin, sensorPin);
  controller = meta->getController();

  //Calibração do modelo
  meta->calibrateLumVoltageModel();
  meta->printModel();
  //Inicialização completa
  Serial.println((char*)"Ready");
  meta->printModel();
  //Init interrupções
  initTimer1();
}

volatile bool flag;
int new_ref;

ISR(TIMER1_COMPA_vect){
  //...
  //put here control operations
  
  // Calcula o sinal de controlo
  controller->calcController();
  
  flag = 1; //notify main loop
 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(flag){
    
    // debug serial
    Serial.print(controller->calSumOtherKus());
    Serial.print("y = ");
    Serial.print(controller->getY(),4);
    Serial.print(';');
    Serial.print("e = ");
    Serial.print(controller->getError(),4);
    Serial.print(';');
    Serial.print("u = ");
    Serial.println(controller->getOwnU(),4); 
    //Actualiza vector para feedforward
    meta->setu_vec();

    // Recebe mensagens 
    SerialCom::receive_message();

    // Obtem a referência da mensagem. è -1 se não houver ref nova.
    meta->setOccupancy(SerialCom::getOccupancy());

    /*if(new_ref != -1){
      controller->setRef(new_ref);
      Serial.println("Mudada Ref");
    }*/

    flag = 0;
  }
}
