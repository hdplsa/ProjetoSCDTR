#ifndef LIGHTSENSOR_H_INCLUDED
#define LIGHTSENSOR_H_INCLUDED

#include "Arduino.h"

class LightSensor{
  //Pin para leitura do sensor de luz
  int PinInput;
  // Referência para o A/D da placa
  const double Vcc = 5.0;
  double AREF;
  //Resistance
  double R = 10000.0;
  //Constantes Calibração
  const double K = -0.6094;
  double a = 94082.4; 
  
  public:
    LightSensor(int PinInput, int AREF);
    double getSensorVoltage();
    double getSensorResistance();
    double getAverageSensorResistance(int N);
    double getLuminousIntensity();
    double getAverageLuminousIntensity(int N);
    ~LightSensor(); // No Fim, se não usado, retirar
    
  private:
    double getPinVoltage(int pin);
    
};

#endif
