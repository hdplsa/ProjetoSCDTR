#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"

char teste[] = "Teste";

void print_char(char* str){
  Serial.println(str);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // Faz setup do I2C

  sei();

  TWI::begin(2);
  TWI::onReceive(print_char);

  SREG |= 0b10000000; // enable interrupts

  //TWCR = (1 << TWEN) | (1 << TWIE);

  //TWI::send_msg(2, teste, 6);

}

void loop() {
  // put your main code here, to run repeatedly:

  delay(2000);
  Serial.println(TWCR,BIN);
  //TWI::send_msg(2, teste, 6);

}
