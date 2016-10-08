#ifndef LUM_H_INCLUDED
#define LUM_H_INCLUDED

#include "LightSensor.h"
#include "LedPWM.h"

class LumItensity{
  //Constantes Calibração
  double K;
  double a;
  //Intensidade luminosa
  double lumIt;
  double k, teta;
  LightSensor *ls;
  LedPWM *ledp;

  public:
    LumItensity(LightSensor *ls);
    double getLuminousItensity();
    void setK(double K);
    void seta(double a);
    void setLedLuminousItensity(double u);
    ~LumItensity();
    
  private:
    void calibrateLumVoltage();
    
};


#endif
