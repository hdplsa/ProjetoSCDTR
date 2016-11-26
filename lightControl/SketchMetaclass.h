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
