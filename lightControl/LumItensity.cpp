#include "LumItensity.h"

// The model for the conversion of R to lux is:
// R = a*lux^k, where 'k' and 'a' are constants
// obtained form the datasheet.

// Constructor, has the default value of Rs, 
// and receives a lightsensor variable,
// from where we can get the LDR's resistance.
LumItensity::LumItensity(LightSensor *ls){
  this->lumIt = 0;
  this->ls = ls;
  this->K = -0.2882;
  this->a = 22000;
  this->ledp = new LedPWM(9,0.085);
  //Calibrações - inicializações
  this->k = 0;
  this->teta = 0;
  this->calibrateLumVoltage();
}

double LumItensity::getLuminousItensity(){
  double R,light;
  //Calcula intensidade luminosa pelos dados do sensor
  R = this->ls->getSensorResistance();
  light = pow(R/this->a,1/this->K);
  return light;
}

void LumItensity::setK(double K){
  this->K = K;
}

void LumItensity::seta(double a){
  this->a = a;
}

void LumItensity::setLedLuminousItensity(double l){
  double u;
  //Modelo matemático l = k*u+teta
  u = (l-this->teta)/this->k;
  this->ledp->setLedPWMVoltage(u);
}

LumItensity::~LumItensity(){
  
}

//Inicialização - Calibração
void LumItensity::calibrateLumVoltage(){
  //Número de testes
  int N = 10;
  //Variáveis experimentais
  double y[N];
  double u[N];
  double us[N];
  //Variáveis Regressão
  int n;
  double sum = 0;
  double sumy = 0;
  double sums = 0;
  double sumyu = 0;
  double det, k, teta;
  //Recolha de dados para regressão linear
  for(n=0;n<=N;n=n+1){
    u[n] = 0.5*n;
    this->ledp->setLedPWMVoltage(u[n]);
    delay(1000);
    y[n] = this->getLuminousItensity();
    /*Serial.print("y[n] = ");
    Serial.print(y[n]);
    Serial.print("\n");*/
  }
  //Regressão Linear (u - entrada, y - saída)
  for(n=0;n<=N;n=n+1){
    sum+=u[n];
    us[n]=u[n]*u[n];
    sums+=us[n];
    sumy+=y[n];
    sumyu+=u[n]*y[n];
  }
  det = 1/(N*sums - sum*sum);
  this->k = det*(N*sumyu - sum*sumy);
  this->teta = det*(-sum*sumyu + sums*sumy);
}

