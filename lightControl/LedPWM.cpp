#include "LedPWM.h"

LedPWM::LedPWM(int LedPin){
  //Pino de saída onde está o LED
  this->LedPin = LedPin;
  //Periodo de onda PWM (s)
  this->TPWM = 1.0/490.0;
}

double LedPWM::getTPWM(){
  return this->TPWM;
}

double LedPWM::getLedPower(){
  return this->PLed;
}

void LedPWM::setLedPWM(int dutCycle){
  //dutCycle representa o duty cycle do PWM (0-255)
  this->setPWMOutput(this->LedPin,dutCycle);
}

void LedPWM::setLedPWMVoltage(double voltage){
  //Passagem de tensão para dutyCycle correspondente no PWM
  this->setPWMOutput(this->LedPin,(int)(voltage*(255.0/5.0)));
}

LedPWM::~LedPWM(){
  
}

void LedPWM::setPWMOutput(int pin, int dutCycle){
  this->dutCycle = dutCycle;
  analogWrite(pin,dutCycle);
}

