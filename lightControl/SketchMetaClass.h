#ifndef META_H_INCLUDED
#define META_H_INCLUDED

#include <string.h>
#include "Arduino.h"
#include "EEPROM.h"
#include "LightController.h"
#include "TWI.h"

// Meta class que encapsula os modulos (classes) necessarios que cada arduino



class Meta{
    
    //Parâmetros de calibracao
    double K11 = 0;
    double K12 = 0;
    double K21 = 0;
    double K22 = 0;
    double theta1 = 0;
    double theta2 = 0;
    //Controlador PID
    LightController *_lightcontroller = 0;
    //String de comunicacao
    char rI2C[20];
    volatile bool sendflag = false;
    
public:
    Meta(double T,int ledPin,int sensorPin);
    LightController *getController();
    void receivedI2C(char *str);
    void calibrateLumVoltageModel();
    void setSendFlag(bool sendflag);
    ~Meta();
    
private:
    char *strAlloc(int len);
    bool First();
    double *MinSquare(const int N, double *u, double *y);
    double Setu(const int N, double u, double PWM);
    double Gety(const int N);
    
};

#endif
