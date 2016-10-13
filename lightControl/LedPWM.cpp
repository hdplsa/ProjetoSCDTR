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

void LedPWM::setLedPWM(int dutCycle){
  //dutCycle representa o duty cycle do PWM (0-255)
  this->setPWMOutput(this->LedPin,dutCycle);
}

void LedPWM::setLedPWMVoltage(double voltage){
  //Passagem de tensão para dutyCycle correspondente no PWM
  this->setPWMOutput(this->LedPin,(int)(voltage*(255.0/5.0)));
  Serial.print("voltage = ");
  Serial.println(voltage,5);
  Serial.print("PWM = ");
  Serial.println((int)(voltage*(255.0/5.0)));
}

double LedPWM::calculateLedEnergyCycle(){
  double DtC,E;
  DtC = this->dutCycle;
  //Calcula energia num ciclo (mJ)
  E = 1000*PLed*(DtC/255.0)*this->TPWM;
  return E;
}

LedPWM::~LedPWM(){
}

void LedPWM::setPWMOutput(int pin, int dutCycle){
  this->dutCycle = dutCycle;
  analogWrite(pin,dutCycle);
}

