#ifndef META_H_INCLUDED
#define META_H_INCLUDED

#include "Arduino.h"
#include "EEPROM.h"
#include "LightController.h"
#include "TWI.h"

// Meta class que encapsula os m�dulos (classes) necess�rios que cada arduino


class Meta{
  
  //Parâmetros de calibra��o
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
