#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // Faz setup do I2C

  char teste[] = "Teste";

  twi_set_SLA((int)3);
  twi_set_slaveR();

  twi_send_msg(3, teste, 6);

}

void loop() {
  // put your main code here, to run repeatedly:

}
