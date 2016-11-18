#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // Faz setup do I2C

  char teste[] = "Teste";

  sei();

  TWI::begin();

  TWI::turn_pullUp();
  TWI::set_rate();

  TWI::set_SLA((int)2);
  TWI::set_slaveR();

  SREG |= 0b10000000; // enable interrupts

  //TWCR = (1 << TWEN) | (1 << TWIE);

  //TWI::send_msg(2, teste, 6);

}

void loop() {
  // put your main code here, to run repeatedly:

  delay(500);
  Serial.println(TWSR,BIN);

}
