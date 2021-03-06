#ifndef META_H_INCLUDED
#define META_H_INCLUDED

#include <string.h>
#include "Arduino.h"
#include "EEPROM.h"
#include "LightController.h"
#include "TWI.h"
#include "Error.h"

#define METADEBUG 0

// Meta class que encapsula os modulos (classes) necessarios que cada arduino

class Meta{

    //Valor do Arduino MASTER
    int *arduinoIndex;
    //Parâmetros de calibração
    int Narduino;
    double *k;
    double theta;
    //Controlador PID
    LightController *_lightcontroller = NULL;
    //String de comunicacao
    volatile char rI2C[32];
    volatile bool sendflag = false;
    volatile bool recvflag = false;
    //Occupancy
    int o = 1;
    
public:
    Meta(int Narduino,int *ArduinoIndex,int ledPin,int sensorPin);
    LightController *getController();
    void receivedI2C(char *str);
    void calibrateLumVoltageModel();
    void printModel();
    void setSendFlag(bool sendflag);
    bool getSendFlag();
    void setu_vec();
    bool getOccupancy();
    double getTheta();
    void setOccupancy(int o);
    ~Meta();
    
private:
    void initEnderecos();
    char *strAlloc(int len);
    void initAllVector(double *v, int dim);
    double calcVectorAverage(double *v, int dim);
    void resetI2CString();
    double *LeastSquare(const int N, double *u, double *y);
    void setLedU(double u);
    double Gety(const int N);    
};

#endif
