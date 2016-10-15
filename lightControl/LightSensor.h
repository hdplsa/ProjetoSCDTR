#ifndef LIGHTSENSOR_H_INCLUDED
#define LIGHTSENSOR_H_INCLUDED

#include "Arduino.h"

class LightSensor{
  //Pin para leitura do sensor de luz
  int PinInput;
  // Referência para o A/D da placa
  double AREF;
  //Resistance
  const double R = 10000.0;
  //Constantes Calibração
  const double K = -0.6227; // Experimental. regressão logarítmica usando 2 pontos.
  const double a = 94082.4; // Experimental. regressão logarítmica usando 2 pontos.
  
  public:
    LightSensor(int PinInput, int AREF);
    double getSensorVoltage();
    double getSensorResistance();
    double getLuminousItensity();
    ~LightSensor(); // No Fim, se não usado, retirar
    
  private:
    double getPinVoltage(int pin);
    
};

#endif
