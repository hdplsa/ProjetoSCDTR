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
  //Variaveis input/output
  this->T = 0;
  this->ref = 0;
  this->e[0] = 0;
  this->e[1] = 0;
  this->e[2] = 0;
  this->y = 0;
  this->u[0] = 0;
  this->u[1] = 0;
  this->satU = 1000;
}

//Inicialização - Calibração
void LightController::calibrateLumVoltage(){
  //Número de testes
  const int N = 10;
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
    u[n] = (5.0/(double)N)*(double)n;
    this->lightoff();
    delay(10);
    this->ledp->setLedPWMVoltage(u[n]);
    delay(100);
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
  Serial.print("K = ");
  Serial.print(this->k);
  Serial.print('\n');
  Serial.print("theta = ");
  Serial.print(this->teta);
  Serial.print("\n\n");
}

void LightController::lightoff(){this->ledp->setLedPWMVoltage(0);}
void LightController::lighton(){this->ledp->setLedPWMVoltage(5);}

void LightController::setT(double T){
  this->T = T;
}

void LightController::setY(double y){
  this->y = y;
}

void LightController::setRef(double ref){
  this->ref = ref;  
}

void LightController::setU(double u){
  this->u[1] = u;
}

void LightController::setSaturation(double satU){
  this->satU = satU;
}

double LightController::getK(){return this->k;}
double LightController::getTeta(){return this->teta;}

double LightController::getControlVariable(){
  return this->u[1];
}

double LightController::calcController(){
  //Avanço do tempo
  this->y = this->getSensorY();
  this->u[0] = this->u[1];
  this->e[0] = this->e[1];
  this->e[1] = this->e[2];
  //Cálculo do sinal de comando neste ciclo
  this->e[2] = this->calcErro();
  this->u[1] = this->calcPController()+this->calcPIController()+this->calcPDController();
  //Saturação na variável de controlo
  if (this->u[1] < 0){
    this->u[1] = 0;
  }
  if (this->u[1] > this->satU){
    this->u[1] = this->satU;
  }
  return this->u[1];
}

void LightController::LEDInputControlVariable(){
  this->ledp->setLedPWMVoltage(this->u[1]);
}

LightController::~LightController(){
  delete this->ls;
  delete this->ledp;
}

double LightController::getSensorY(){
  this->y = ls->getLuminousItensity();
}

double LightController::calcErro(){
  //Cálcula erro de entrada no Controlador
  this->e[1] = this->ref-this->y;
  return this->e[1];
}

double LightController::calcPController(){
  //Retorno do controlo proporcional
  return this->Kp*this->e[1];
}

double LightController::calcPIController(){
  double u;
  //Controlo proporcional integral
  u = this->u[0]+e[1]*this->Ki*this->T;
  return u;
}

double LightController::calcPDController(){
  double u;
  //Controlo proporcional diferencial
  u = this->Kd*((this->e[2]-this->e[1])/this->T);
  return u;
}

