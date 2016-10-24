#include <avr/io.h>
#include <avr/interrupt.h>
#include "SerialCom.h"
#include "LightController.h"

SerialCom *serialcom;
LightController * controller;

const int ledPin = 11;
const int sensorPin = 5;

void initTimer1(){
  cli();
  TCCR1A = 0;// clear register
  TCCR1B = 0;// clear register
  TCNT1 = 0;//reset counter
  OCR1A = 3124; //must be <65536
  // = (16*10^6) / (1*1024) – 1
  TCCR1B |= (1 << WGM12); //CTC On
  // Set prescaler for 1024
  TCCR1B |= (1<<CS12)|(1<<CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); //allow interrupts
}

void setup() {
  // put your setup code here, to run once:
  
  serialcom = new SerialCom(9600);

  controller = new LightController(ledPin, sensorPin);
  controller->setT(0.2);
  controller->setRef(0);
  controller->setSaturation(5);
  
  serialcom->send_message((char*)"Ready");

   //Init interrupções
   initTimer1();
}

volatile bool flag;
long unsigned int times1;
long unsigned int times2;

ISR(TIMER1_COMPA_vect){
  //...
  //put here control operations
  //
  flag = 1; //notify main loop
 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(flag){

    /*times1 = times2;
    times2 = millis();
    
    Serial.println(times2 - times1);*/
    controller->calcController();
    controller->LEDInputControlVariable();

    flag = 0;
  }
}
