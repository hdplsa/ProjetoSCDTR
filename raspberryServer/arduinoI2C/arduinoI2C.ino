#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"
#include "EEPROM.h"
#include "string.h"

char teste[] = "Teste";

int count = 0;

void print_char(char* str){
  Serial.println("Total:");
  Serial.println(str);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // Faz setup do I2C

  sei();

  TWI::begin(EEPROM.read(0));
  TWI::onReceive(print_char);

  SREG |= 0b10000000; // enable interrupts

  Serial.println(strlen(teste));

  delay(5000);

}

void loop() {
  // put your main code here, to run repeatedly:


  if(count % 2 == 0){
    if(EEPROM.read(0) == 10){
      TWI::send_msg(11, teste, strlen(teste));
    } 
  } else {
    if(EEPROM.read(0) == 11){
      TWI::send_msg(11, teste, strlen(teste));
    } 
  }
  delay(10000);

}
