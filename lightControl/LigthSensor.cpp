#include "LightSensor.h"

LightSensor::LightSensor(int PinInput){
  this->PinInput = PinInput;
  this->RPin = 0;
  this->R = 10000;
}

double LightSensor::getSensorVoltage(){
  return getPinVoltage(this->PinInput);
}

double LightSensor::getSensorResistance(){
  double U, I, R;
  I = this->getPinVoltage(this->RPin)/this->R;
  U = getSensorVoltage();
  R = (5-U)/I;
  return R;
}


LightSensor::~LightSensor(){
  
}

double LightSensor::getPinVoltage(int pin){
  double rd;
  rd = analogRead(this->PinInput)*(5000/1024);
  return rd/1000;
}
