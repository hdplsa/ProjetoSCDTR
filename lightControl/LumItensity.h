#ifndef LUM_H_INCLUDED
#define LUM_H_INCLUDED

#include "LightSensor.h"

class LumItensity{
  //Resistência do sensor
  double Rs;
  //Intensidade luminosa
  double lumIt;
  LightSensor *ls;

  public:
    LumItensity(LightSensor *ls);
    void setSensorResistance(double Rs);
    double getLuminousItensity();
    ~LumItensity();
    
  private:
};


#endif
