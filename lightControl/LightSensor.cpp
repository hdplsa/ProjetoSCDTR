#include "LightSensor.h"

// Construtor
LightSensor::LightSensor(int PinInput){
  this->PinInput = PinInput;
  this->R = 10000.0;
}

double LightSensor::getSensorVoltage(){
  return getPinVoltage(this->PinInput);
}

double LightSensor::getSensorResistance(){
  double U, I, R;
  U = getSensorVoltage();
  I = U/this->R;
  R = (5.0-U)/I;
  return R;
}

LightSensor::~LightSensor(){
  
}

double LightSensor::getPinVoltage(int pin){
  double rd;
  rd = analogRead(this->PinInput)*(5.0/1024.0);
  return rd;
}
