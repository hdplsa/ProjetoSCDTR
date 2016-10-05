#ifndef LIGHTSENSOR_H_INCLUDED
#define LIGHTSENSOR_H_INCLUDED

#include "Arduino.h"

class LightSensor{
  //Pin para leitura do sensor de luz
  int PinInput;
  int RPin;
  //Resistance
  int R;
  
  public:
    LightSensor(int PinInput);
    double getSensorVoltage();
    double getSensorResistance();
    ~LightSensor();
    
  private:
    double getPinVoltage(int pin);
    
};

#endif
