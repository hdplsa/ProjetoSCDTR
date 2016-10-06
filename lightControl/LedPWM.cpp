#include "LedPWM.h"

LedPWM::LedPWM(int LedPin,double PLed){
  this->LedPin = LedPin;
  this->PLed = PLed;
  this->TPWM = 1/490;
}

double LedPWM::getTPWM(){
  return this->TPWM;
}

void LedPWM::setLedPWM(int value){
  this->setPWMOutput(this->LedPin,value);
}

double LedPWM::calculateLedEnergyCycle(){
  double E, d;
  E = PLed*(this->dutCycle/255)*this->TPWM;
  return E;
}

LedPWM::~LedPWM(){
  
}


void LedPWM::setPWMOutput(int pin, int dutCycle){
  this->dutCycle = dutCycle;
  analogWrite(pin,dutCycle);
}

