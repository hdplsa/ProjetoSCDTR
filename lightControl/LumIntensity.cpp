#include "LumIntensity.h"

// The model for the conversion of R to lux is:
// R = a*lux^k, where 'k' and 'a' are constants
// obtained form the datasheet.

// Constructor, has the default value of Rs, 
// and receives a lightsensor variable,
// from where we can get the LDR's resistance.
LumIntensity::LumIntensity(int ledPin, int sensorPin){
  this->lumIt = 0;
  this->ls = new LightSensor(sensorPin, 5.0);
}

double LumIntensity::getLuminousItensity(){
  double R,light;
  //Calcula intensidade luminosa pelos dados do sensor
  R = this->ls->getSensorResistance();
  light = pow(R/this->a,1/this->K);
  return light;
}

/*void LumIntensity::setLedLuminousItensity(double l){
  double u;
  //Modelo matemático l = k*u+teta
  u = (l-this->teta)/this->k;
  this->ledp->setLedPWMVoltage(u);
}*/

LumIntensity::~LumIntensity(){
  
}

