#include "LightSensor.h"
#include "LedPWM.h"
#include "SerialCom.h"
#include "LightController.h"

LightSensor *sensor;
SerialCom *serialcom;
LightController * controller;

const int ledPin = 9;
const int sensorPin = 5;

void setup() {
  // put your setup code here, to run once:

  serialcom = new SerialCom(9600);
  serialcom->send_message((char*)"Ready");

  controller = new LightController(ledPin, sensorPin,0,0,0);
  
  Serial.print(controller->getK());
  Serial.print('\n');
  Serial.print(controller->getTeta());
  Serial.print("\n\n");
}

void loop() {
  // put your main code here, to run repeatedly:

 
  delay(2000);

}
