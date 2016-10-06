#include "LumItensity.h"

// The model for the conversion of R to lux is:
// R = a*lux^k, where 'k' and 'a' are constants
// obtained form the datasheet.

const double k = -0.2882;
const double a = 22000;

//const double k = -1.0328;
//const double a = 70000;


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
  light = pow(R/a,1/k);
  return light;
}

LumItensity::~LumItensity(){
  
}

