#ifndef LUM_H_INCLUDED
#define LUM_H_INCLUDED

#include "LightSensor.h"

class LumItensity{
  //Constantes Calibração
  double K;
  double a;
  //Intensidade luminosa
  double lumIt;
  LightSensor *ls;

  public:
    LumItensity(LightSensor *ls);
    double getLuminousItensity();
    void setK(double K);
    void seta(double a);
    ~LumItensity();
    
  private:
    
};


#endif
