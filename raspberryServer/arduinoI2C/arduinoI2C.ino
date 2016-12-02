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

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // Faz setup do I2C

  sei();

  TWI::begin(EEPROM.read(0));
  TWI::onReceive(print_char);
  TWI::set_DEBUG();

  SREG |= 0b10000000; // enable interrupts

  Serial.println(strlen(teste));

  delay(5000);

}

void loop() {
  // put your main code here, to run repeatedly:

  if(EEPROM.read(0) == 10){
    TWI::send_msg(11, teste, strlen(teste));
  } 

  delay(10000);
}
