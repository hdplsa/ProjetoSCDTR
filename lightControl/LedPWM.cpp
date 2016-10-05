#include "LedPWM.h"

LedPWM::LedPWM(int LedPin){
  this->LedPin = LedPin;
}

void LedPWM::setLedPWM(int value){
  this->setPWMOutput(this->LedPin,value);
}

LedPWM::~LedPWM(){
  
}


void LedPWM::setPWMOutput(int pin, int value){
  analogWrite(pin,value);
}

