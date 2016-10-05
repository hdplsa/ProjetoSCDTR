#ifndef LEDPWM_H_INCLUDED
#define LEDPWM_H_INCLUDED

#include "Arduino.h"


class LedPWM{
  //Pin do LED
  int LedPin;
  
  public:
    LedPWM(int LedPin);
    void setLedPWM(int value);
    ~LedPWM();
    
  private:
    void setPWMOutput(int pin, int value);
};

#endif
