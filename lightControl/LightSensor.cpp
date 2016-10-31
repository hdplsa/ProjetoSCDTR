#include "LightSensor.h"

// Construtor
LightSensor::LightSensor(int PinInput, int AREF){
  this->PinInput = PinInput;
  this->AREF = AREF;
  this->R = 10000.0;
  this->K = -0.6094;
  this->a = 94082.4;
}

double LightSensor::getSensorVoltage(){
  return getPinVoltage(this->PinInput);
}

// Obtém a tensão, calcula a corrente (U=RI), e por fim a resistência 
// do LDR.
double LightSensor::getSensorResistance(){
  double U, I, R;
  U = getSensorVoltage();
  I = U/this->R;
  R = (this->Vcc-U)/I;
  return R;
}

double LightSensor::getAverageSensorResistance(int N){
  double n,av;
  for(n=1,av=0;n<=N;n++){
    av += this->getSensorResistance();
  }
  av = av/N;
  return av;
}

// The model for the conversion of R to lux is:
// R = a*lux^k, where 'k' and 'a' are constants
// obtained form the datasheet.

// Constructor, has the default value of Rs, 
// and receives a lightsensor variable,
// from where we can get the LDR's resistance.
double LightSensor::getLuminousIntensity(){
  double R,light;
  //Calcula intensidade luminosa pelos dados do sensor
  R = this->getSensorResistance();
  light = pow((double)(R/this->a),(double)(1.0/this->K));
  return light;
}

double LightSensor::getRaw(){
  return analogRead(this->PinInput);
}

//Retorna média de N amostras do sensor
double LightSensor::getAverageLuminousIntensity(int N){
  double av, i;
  for(i=1,av=0;i<=N;i++){
    av += this->getLuminousIntensity();
  }
  av = av/N;
  return av;
}

LightSensor::~LightSensor(){
  
}

// Lê a tensão do pin que está no range [0,1024[, e converte para volts.
// Atenção em mudar o AREF no construtor!!
double LightSensor::getPinVoltage(int pin){
  double rd;
  rd = analogRead(this->PinInput)*(this->AREF/1024.0);
  return rd;
}
