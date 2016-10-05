#include "LumItensity.h"

// The model for the conversion of R to lux is:
// R = a*lux^k, where 'k' and 'a' are constants
// obtained form the datasheet.

const double k = 0.5294;
const double a = log(25000);

// Constructor, has the default value of Rs, 
// and receives a lightsensor variable,
// from where we can get the LDR's resistance.
LumItensity::LumItensity(LightSensor *ls){
  this->lumIt = 0;
  this->ls = ls;
}

double LumItensity::getLuminousItensity(){
  double R,light;
  //Calcula intensidade luminosa pelos dados do sensor
  R = this->ls->getSensorResistance();
  light = log(R/a)/log(k);
  return light;
}

LumItensity::~LumItensity(){
  
}

