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
  double R;
  //Constantes Calibração
  double K; //log10(300/800)/log10(100/20); // Experimental. regressão logarítmica usando 2 pontos.
  double a; //pow(10,log10(800e3) - K*log10(20)); // Experimental. regressão logarítmica usando 2 pontos.
  
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
