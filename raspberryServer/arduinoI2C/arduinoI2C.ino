#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"
#include "EEPROM.h"
#include "string.h"

char teste[] = "Teste";
char hoe[] = "hoe";

int count = 0;

void print_char(char* str){
  Serial.println("Total:");
  Serial.println(str);
  // Teste o slave mandar uma mensagem.
  // Ou seja, o slave receiver do estado anterior
  // entra em master transmiter
  if(EEPROM.read(0) == 11){
    TWI::send_msg(10, hoe, strlen(hoe));
  }
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

  char msg[] = "a\n";

  count = 1;

  // Itera sobre todos os endereços
  for(int i = 10; i <= 120; i++){

    delay(100);

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

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  EEPROM.write(0,12);

  // Faz setup do I2C

  sei();

  TWI::begin(EEPROM.read(0));
  TWI::onReceive(print_char);
  TWI::onSend(count_I2Csend);
  TWI::onSendError(count_I2CsendError);

  SREG |= 0b10000000; // enable interrupts

  Serial.println(strlen(teste));

  delay(5000);

}

void loop() {
  // put your main code here, to run repeatedly:


  Serial.println(count_TWI());

  delay(10000);
}
