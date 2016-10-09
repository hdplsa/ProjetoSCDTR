#ifndef LUM_H_INCLUDED
#define LUM_H_INCLUDED

#include "LightSensor.h"
#include "LedPWM.h"

class LumIntensity{
  //Constantes Calibração
  const double K = -0.2882;
  const double a = 22000;
  //Intensidade luminosa
  double lumIt;
  LightSensor *ls;

  public:
    LumIntensity(int ledPin, int sensorPin);
    double getLuminousItensity();
    void setK(double K);
    void seta(double a);
    void setLedLuminousItensity(double u);
    ~LumIntensity();
    
  private:
    
};


#endif
