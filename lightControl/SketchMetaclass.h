#ifndef META_H_INCLUDED
#define META_H_INCLUDED

#include "Arduino.h"
#include "EEPROM.h"
#include "LightController.h"
#include "TWI.h"

// Meta class que encapsula os modulos (classes) necessarios que cada arduino


class Meta{
  
  //Parâmetros de calibra��o
  double K11 = 0;
  double K12 = 0;
  double K21 = 0;
  double K22 = 0;
  double theta1 = 0;
  double theta2 = 0;
  //Controlador PID
  LightController *_lightcontroller = 0;

  public:
    Meta(int ledPin,int sensorPin);
    void calibrateLumVoltageModel();
    ~Meta();
  
  private:
    bool defineMaster();
};

#endif
