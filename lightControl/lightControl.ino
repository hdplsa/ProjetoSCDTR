#include <avr/io.h>
#include <avr/interrupt.h>
#include "SerialCom.h"
#include "LightController.h"

SerialCom *serialcom;
LightController * controller;

constexpr int ledPin = 11;
constexpr int sensorPin = 5;

constexpr uint8_t deviceID = 2;

void initTimer1(){
  cli();
  TCCR1A = 0;// clear register
  TCCR1B = 0;// clear register
  TCNT1 = 0;//reset counter
  OCR1A = 313; //must be <65536
  // = (16*10^6) / (1*1024) – 1
  TCCR1B |= (1 << WGM12); //CTC On
  // Set prescaler for 1024
  TCCR1B |= (1<<CS12)|(1<<CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); //allow interrupts
}

/* Vai dizer ao arduino o seu endereço no I2C e 
 * inicià-lo como slave receiver
 */
void initI2C(){

  // Define o numero do dispositivo
  TWAR = deviceID;

  // Liga o TWIN e liga a deteção do address do arduino
  TWCR |= 0b01000100;
  
}

void setup() {
  // put your setup code here, to run once:
  
  serialcom = new SerialCom(115200);

  controller = new LightController(ledPin, sensorPin);
  controller->setT(0.02);
  controller->setRef(50);
  controller->setSaturation(5);

  Serial.println((char*)"Ready");

  //Init interrupções
  initTimer1();
}

volatile bool flag;
int new_ref;

ISR(TIMER1_COMPA_vect){
  //...
  //put here control operations
  
  // Calcula o sinal de controlo
  controller->calcController();
  
  flag = 1; //notify main loop
 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(flag){
    
    // debug serial
    Serial.print("y = ");
    Serial.print(controller->getY(),4);
    Serial.print(';');
    Serial.print("e = ");
    Serial.print(controller->getError(),4);
    Serial.print(';');
    Serial.print("u = ");
    Serial.print(controller->getControlVariable(),4);
    Serial.print(';');
    Serial.print("t = ");
    Serial.println(millis()); 

    // Recebe mensagens 
    serialcom->receive_message();

    // Obtem a referência da mensagem. è -1 se não houver ref nova.
    new_ref = serialcom->getRef();

    if(new_ref != -1){
      controller->setRef(new_ref);
      Serial.println("Mudada Ref");
    }

    flag = 0;
  }
}
