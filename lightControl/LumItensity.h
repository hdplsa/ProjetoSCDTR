#ifndef LUM_H_INCLUDED
#define LUM_H_INCLUDED

#include "LightSensor.h"

class LumItensity{
  //Intensidade luminosa
  double lumIt;
  LightSensor *ls;

  public:
    LumItensity(LightSensor *ls);
    double getLuminousItensity();
    ~LumItensity();
    
  private:
    
};


#endif
