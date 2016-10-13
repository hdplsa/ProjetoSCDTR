#include "LightController.h"

LightController::LightController(int ledPin, int sensorPin, double Kp,double Ki,double Kd){
  //Parâmetros do controlador
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;
  //Depêndencias do feedback
  this->ls = new LightSensor(sensorPin,5);
  this->ledp = new LedPWM(ledPin);
  //Variáveis do modelo
  this->k = 0;
  this->teta = 0;
  this->calibrateLumVoltage();
  this->lightoff();
  delay(100);
}

LightController::~LightController(){
  delete this->ls;
  delete this->ledp;
}

//Inicialização - Calibração
void LightController::calibrateLumVoltage(){
  //Número de testes
  const int N = 5;
  //Variáveis experimentais
  double y[N];
  double u[N];
  double usquare[N];
  double cumsum = 0;
  //Variáveis Regressão
  double sum = 0;
  double sumy = 0;
  double sumsquare = 0;
  double sumyu = 0;
  double det;
  //Recolha de dados para regressão linear
  for(int n=0;n<N;n++){
    u[n] = (5.0/N)*n;
    this->lightoff();
    delay(2000);
    this->ledp->setLedPWMVoltage(u[n]);
    delay(2000);
    for(int j=0;j<=10;j++){cumsum += this->ls->getLuminousItensity();}
    y[n] = cumsum/10;
    cumsum = 0;
    Serial.print(n);
    Serial.print(' ');
    Serial.print(y[n]);
    Serial.print('\n');
  }

  //Regressão Linear (u - entrada, y - saída) mínimos quadrados
  for(int n=0;n<N;n++){
    sum+=u[n];
    usquare[n]=u[n]*u[n];
    sumsquare+=usquare[n];
    sumy+=y[n];
    sumyu+=y[n]*u[n];
  }
  //Modelo matemático l = k*u+teta
  det = 1/(N*sumsquare - sum*sum);
  this->k = det*(N*sumyu - sum*sumy);
  this->teta = det*(-sum*sumyu + sumsquare*sumy);

  //Debug Stuff. descomentar quando necessário
  /*Serial.print(this->k);
  Serial.print('\n');
  Serial.print(this->teta);
  Serial.print("\n\n");*/
}

void LightController::lightoff(){this->ledp->setLedPWMVoltage(0);}
void LightController::lighton(){this->ledp->setLedPWMVoltage(5);}
double LightController::getK(){return this->k;}
double LightController::getTeta(){return this->teta;}

