#ifndef LIGHTSENSOR_H_INCLUDED
#define LIGHTSENSOR_H_INCLUDED

#include "Arduino.h"

class LightSensor{
  //Pin para leitura do sensor de luz
  int PinInput;
  int RPin;
  // Referência para o A/D da placa
  double AREF;
  //Resistance
  const double R = 10000.0;
  //Constantes Calibração
  const double K = -0.2882;
  const double a = 22000;
  
  public:
    LightSensor(int PinInput, int AREF);
    double getSensorVoltage();
    double getSensorResistance();
    double getLuminousItensity();
    ~LightSensor();
    
  private:
    double getPinVoltage(int pin);
    
};

#endif
