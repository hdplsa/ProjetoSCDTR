#include "LightSensor.h"
#include "LedPWM.h"

LightSensor *ls;
LedPWM *Ledp;

void setup() {
  // put your setup code here, to run once:
  ls = new LightSensor(0);
  Ledp = new LedPWM(11);
}

void loop() {
  // put your main code here, to run repeatedly:

}
