#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // Faz setup do I2C

  char teste[] = "Teste";

  sei();

  twi_turn_pullUp();
  twi_set_rate();

  //twi_set_SLA((int)2);
  //twi_set_slaveR();

  SREG |= 0b10000000; // enable interrupts

  //TWCR = (1 << TWEN) | (1 << TWIE);

  twi_send_msg(2, teste, 6);

}

void loop() {
  // put your main code here, to run repeatedly:

}
