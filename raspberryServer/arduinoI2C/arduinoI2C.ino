#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"
#include "EEPROM.h"
#include "string.h"

char teste[] = "Teste";

void print_char(char* str){
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

  //TWCR = (1 << TWEN) | (1 << TWIE);

  //TWI::send_msg(2, teste, 6);

}

void loop() {
  // put your main code here, to run repeatedly:

  if(EEPROM.read(0) == 10){
    TWI::send_msg(11, (char*) "Teste\n", strlen("Teste\n"));
  }

  delay(100000);

}
