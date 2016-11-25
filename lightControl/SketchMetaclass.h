#ifndef META_H_INCLUDED
#define META_H_INCLUDED

#include "Arduino.h"
#include "EEPROM.h"
#include "LightController.h"
#include "TWI.h"

const double Vcc = 5.0;

// Meta class que encapsula os módulos (classes) necessários que cada arduino


class Meta{
  
  //ParÃ¢metros de calibração
  double K11;
  double K12;
  double K21;
  double K22;
  double theta1;
  double theta2;
  
  LightController *_lightcontroller;

  public:
    Meta(int ledPin,int sensorPin);
    void calibrateLumVoltage();
    ~Meta();
  
  private:
    bool DefineMaster();
};

#endif
