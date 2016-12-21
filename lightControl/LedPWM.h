#ifndef LEDPWM_H_INCLUDED
#define LEDPWM_H_INCLUDED

#include "Arduino.h"


class LedPWM{
  //Pin do LED
  int LedPin;
  //Energia do LED
  int dutCycle;
  double TPWM;
  
  public:
    LedPWM(int LedPin);
    double getTPWM();
    double getLedPower();
    void setLedPWM(int dutCycle);
    void setLedPWMVoltage(double voltage);
    ~LedPWM();
    
  private:
    void setPWMOutput(int pin, int dutCycle);
};

#endif
