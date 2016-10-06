#include "LightSensor.h"
#include "LedPWM.h"
#include "LumItensity.h"

LightSensor *ls;
LedPWM *Ledp;
LumItensity *luminten;

const int ledPin = 9;
const int sensorPin = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ls = new LightSensor(sensorPin);
  Ledp = new LedPWM(ledPin);
  luminten = new LumItensity(ls);
}

void loop() {
  // put your main code here, to run repeatedly:

  double R, lux;
  
  Ledp->setLedPWM(100);
  R = ls->getSensorResistance();
  lux = luminten->getLuminousItensity();

  Serial.print("R = ");
  Serial.print(R);
  Serial.print("\n");
  Serial.print("lux = ");
  Serial.print(lux);
  Serial.print("\n");

  delay(1000);
}
