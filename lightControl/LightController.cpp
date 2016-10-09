#include "LightController.h"

LightController::LightController(int ledPin, int sensorPin, double Kp,double Ti,double Td){
  //Parâmetros do controlador
  this->Kp = Kp;
  this->Ti = Ti;
  this->Td = Td;
  //Depêndencias do feedback
  this->ls = new LightSensor(sensorPin,5);
  this->ledp = new LedPWM(ledPin);
  //Variáveis do modelo
  this->k = 0;
  this->teta = 0;
  this->calibrateLumVoltage();
}

LightController::~LightController(){
  delete this->ls;
  delete this->ledp;
}

//Inicialização - Calibração
void LightController::calibrateLumVoltage(){
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
  double det;
  //Recolha de dados para regressão linear
  for(n=0;n<=N;n=n+1){
    u[n] = 0.5*n;
    this->ledp->setLedPWMVoltage(u[n]);
    delay(1000);
    y[n] = this->ls->getLuminousItensity();
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
  //Modelo matemático l = k*u+teta
  det = 1/(N*sums - sum*sum);
  this->k = det*(N*sumyu - sum*sumy);
  this->teta = det*(-sum*sumyu + sums*sumy);
}

