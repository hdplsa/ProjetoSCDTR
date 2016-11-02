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
  OCR1A = 3*1562; //must be <65536
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

  serialcom = new SerialCom(115200);
  
  controller = new LightController(ledPin, sensorPin);
  controller->setT(0.2);
  controller->setRef(50);
  controller->setSaturation(5);
  controller->setPWM(51);
  
  Serial.print((char*)"Ready\n");

  //Init interrupções
  //initTimer1();
}

const int N = 200;
volatile bool send_flag = 1;
int y[N];
int cycle = 0;
long unsigned int t[N];

ISR(TIMER1_COMPA_vect){
  //...
  //put here control operations
  //
  //flag = 0; //notify main loop
  if(cycle <= 25){
    //y = controller->getSensorRaw();
    //u = cycle*10;

    //Serial.print(y);
    //Serial.print(';');
    //Serial.print(u);
    //Serial.print('\n');
    
    //controller->setPWM(cycle*10);
    //cycle++;
  } else {
    //flag = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(cycle < N){

    y[cycle] = controller->getSensorRaw();
    t[cycle] = micros();

  } else {
    if(send_flag){
      for(int i=0; i<N; i++){
        Serial.print(y[i]);
        Serial.print(';');
        Serial.print(t[i]);
        Serial.print('\n');
      }
      send_flag = 0;
    }
  }
  
  cycle++;
  delayMicroseconds(1200);  
}
