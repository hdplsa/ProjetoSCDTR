#include "LedPWM.h"

LedPWM::LedPWM(int LedPin,double PLed){
  //Pino de saída onde está o LED
  this->LedPin = LedPin;
  //Potência do LED (W)
  this->PLed = PLed;
  //Periodo de onda PWM (s)
  this->TPWM = 1.0/490.0;
}

double LedPWM::getTPWM(){
  return this->TPWM;
}

void LedPWM::setLedPWM(int value){
  this->setPWMOutput(this->LedPin,value);
}

double LedPWM::calculateLedEnergyCycle(){
  double DtC;
  DtC = this->dutCycle;
  return PLed*(DtC/255.0)*this->TPWM;
}

LedPWM::~LedPWM(){
  
}

void LedPWM::setPWMOutput(int pin, int dutCycle){
  this->dutCycle = dutCycle;
  analogWrite(pin,dutCycle);
}

