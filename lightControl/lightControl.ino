#include "SerialCom.h"
#include "LightController.h"

SerialCom *serialcom;
LightController * controller;

const int ledPin = 9;
const int sensorPin = 5;

void setup() {
  // put your setup code here, to run once:

  serialcom = new SerialCom(9600);

  controller = new LightController(ledPin, sensorPin,1,1,0);
  controller->setT(0.5);
  controller->setRef(50);
  controller->setSaturation(5);
  
  serialcom->send_message((char*)"Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  controller->calcController();
  controller->LEDInputControlVariable();
  delay(controller->getT()*1000);
}
