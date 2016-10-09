#include "LightSensor.h"
#include "LedPWM.h"
#include "LumIntensity.h"
#include "SerialCom.h"

LumIntensity *luminten;
SerialCom *serialcom;

const int ledPin = 9;
const int sensorPin = 5;

void setup() {
  // put your setup code here, to run once:
  
  luminten = new LumIntensity(ledPin, sensorPin);
  serialcom = new SerialCom(9600, luminten);
  serialcom->send_message((char*)"Ready");
}

// funções auxiliares

void reset_board_serial(void (f())){

  int incoming;
  
  // Se ele tiver uma ligação serial, e receber 42,
  // Executa a função.
  if(Serial.available() > 0){

    incoming = Serial.read();

    if(incoming == 42){

      f();
      
    }  
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  //double R, lux;
  
  //Ledp->setLedPWM(100);
  //R = ls->getSensorResistance();
  //lux = luminten->getLuminousItensity();

  //Serial.print("R = ");
  //Serial.print(R);
  //Serial.print("\n");
  //sprintf(buffer,"lux = %i \n",16);
  //serialcom->send_message(buffer);
  //Serial.print(lux);
  //Serial.print("\n");
  //Serial.print("Energia = ");
  //Serial.print(Ledp->calculateLedEnergyCycle(),4);
  //Serial.print("\n");

  delay(50);

  serialcom->receive_message();

  }
