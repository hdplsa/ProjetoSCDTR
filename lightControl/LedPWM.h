#ifndef LEDPWM_H_INCLUDED
#define LEDPWM_H_INCLUDED

#include "Arduino.h"


class LedPWM{
  //Pin do LED
  int LedPin;
  //Energia do LED
  int dutCycle;
  double PLed;
  double TPWM;
  
  public:
    LedPWM(int LedPin,double PLed);
    double getTPWM();
    void setLedPWM(int value);
    double calculateLedEnergyCycle();
    ~LedPWM();
    
  private:
    void setPWMOutput(int pin, int dutCycle);
};

#endif
