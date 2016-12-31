#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"
#include "EEPROM.h"
#include "string.h"

#define COUNT 100

char teste[] = "Teste";
char hoe[] = "hoe";

volatile int count = 0;
volatile bool received = false;

long t1 = 0;
long t2 = 0;

void print_char(char* str){

  if(EEPROM.read(0) == 11){
    TWI::send_msg(10, (char*)'\n', 1);
  } 
  received = true;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  //EEPROM.write(0,12);

  // Faz setup do I2C

  sei();

  TWI::begin(EEPROM.read(0));
  TWI::onReceive(print_char);

  SREG |= 0b10000000; // enable interrupts

  Serial.println(EEPROM.read(0));

  delay(5000);

  t1 = millis();

}

void loop() {
  // put your main code here, to run repeatedly:

  if(EEPROM.read(0) == 10){
    TWI::send_msg(11, (char*)'\n', 1);
  }
  
  while(!received){};
  received = false;

  count ++;
 
  if(count == COUNT){
    t2 = millis();
    Serial.println(t2-t1);
    delay(10000);
    t1 = millis();
    count = 0;
  }

}
