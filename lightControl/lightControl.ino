#include "LightSensor.h"
#include "LedPWM.h"

LightSensor *ls;
LedPWM *Ledp;

const int ledPin = 9;
const int sensorPin = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ls = new LightSensor(sensorPin);
  Ledp = new LedPWM(ledPin);
}

void loop() {
  // put your main code here, to run repeatedly:

  double R;
  
  Ledp->setLedPWM(100);
  R = ls->getSensorResistance();

  Serial.print("R = ");
  Serial.print(R);
  Serial.print("\n");

  delay(1000);
}
