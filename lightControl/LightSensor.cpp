#include "LightSensor.h"

// Construtor
LightSensor::LightSensor(int PinInput, int AREF){
  this->PinInput = PinInput;
  this->AREF = AREF;
}

double LightSensor::getSensorVoltage(){
  return getPinVoltage(this->PinInput);
}

// Obtém a tensão, calcula a corrente (U=RI), e por fim a resistência 
// do LDR.
double LightSensor::getSensorResistance(){
  double U, I, R;
  U = getSensorVoltage();
  I = U/this->R;
  R = (5.0-U)/I;
  return R;
}

// The model for the conversion of R to lux is:
// R = a*lux^k, where 'k' and 'a' are constants
// obtained form the datasheet.

// Constructor, has the default value of Rs, 
// and receives a lightsensor variable,
// from where we can get the LDR's resistance.
double LightSensor::getLuminousItensity(){
  double R,light;
  //Calcula intensidade luminosa pelos dados do sensor
  R = this->getSensorResistance();
  light = pow(R/this->a,1/this->K);
  return light;
}

LightSensor::~LightSensor(){
  
}

// Lê a tensão do pin que está no range [0,1024[, e converte para volts.
// Atenção em mudar o AREF no construtor!!
double LightSensor::getPinVoltage(int pin){
  double rd;
  rd = analogRead(this->PinInput)*(this->AREF/1024.0);
  return rd;
}
