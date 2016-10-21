#include "LightController.h"

LightController::LightController(int ledPin, int sensorPin){
  //Parâmetros do controlador
  this->Kp = 0.001;
  this->Ki = 0.01;
  this->Kd = 0;
  //Depêndencias do feedback
  this->ls = new LightSensor(sensorPin,5);
  this->ledp = new LedPWM(ledPin);
  //Variáveis do modelo tensão/lux
  this->k = 0;
  this->teta = 0;
  this->calibrateLumVoltage();
  //Variaveis input/output
  this->T = 0;
  this->ref = 0;
  this->e[0] = 0;
  this->e[1] = 0;
  this->y = 0;
  this->u[0] = 0;
  this->u[1] = 0;
  this->ui_ant = 0;
  this->ud_ant = 0;
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
    this->ledp->setLedPWMVoltage(u[n]);
    delay(100);
    y[n] = this->ls->getAverageLuminousIntensity(10);
    //Prints de debug
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

  //Desligar a luz no fim
  this->lightoff();

  //Debug Stuff. descomentar quando necessário
  /*Serial.print("K = ");
  Serial.print(this->k);
  Serial.print('\n');
  Serial.print("theta = ");
  Serial.print(this->teta);
  Serial.print("\n\n");*/
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
  this->u[2] = u;
}

void LightController::setSaturation(double satU){
  this->satU = satU;
}

double LightController::getK(){return this->k;}
double LightController::getTeta(){return this->teta;}

double LightController::getT(){
  return this->T;  
}

double LightController::getControlVariable(){
  return this->u[2];
}

/* Legenda dos sinais:
    e[1] -> sinal de erro no ciclo atual
    e[0] -> sinal de erro no ciclo -1

    u[1] -> sinal de controlo no ciclo atual
    u[0] -> sinal de controlo no ciclo -1
   
    ui_ant -> sinal de controlo do controlador integral do ciclo passado
    ud_ant -> sinal de controlo do controlador diferencial do ciclo anterior

*/

double LightController::calcController(){

  double up, ui, ud;
  
  // Avanço do tempo das samples dos sinais
  this->y = this->getSensorY();
  this->u[0] = this->u[1];
  this->e[0] = this->e[1];
  
  // Cálculo do sinal de erro
  this->e[1] = this->calcErro();

  // Cálculo do sinal de controlo  
  up = this->calcPController();
  ui = this->calcPIController();
  ud = this->calcPDController();
  
  this->u[1] = up + ui + ud;
  
  // Saturação na variável de controlo
  if (this->u[1] < 0){
    this->u[1] = 0;
  }
  if (this->u[1] > this->satU){
    this->u[1] = this->satU;
  }
  
  // debug serial
  Serial.print("y = ");
  Serial.println(this->y,4);
  Serial.print("e = ");
  Serial.println(e[2],4);
  Serial.print("u = ");
  Serial.println(u[2],4);
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
  double y;
  y = ls->getLuminousIntensity();
  return y;
}

double LightController::calcErro(){
  //Cálcula erro de entrada no Controlador
  this->e[2] = this->ref-this->y;
  return this->e[2];
}

double LightController::calcPController(){
  //Retorno do controlo proporcional
  return this->Kp*this->e[2];
}

double LightController::calcPIController(){
  double u;
  //Controlo proporcional integral
  u = this->Ki*((this->T*0.5)*(this->e[1]+this->e[0])) + this->ui_ant;
  return u;
}

double LightController::calcPDController(){
  double u;
  //Controlo proporcional diferencial
  u = this->Kd*((2/this->T)*(this->e[1]-this->e[0])) - this->ud_ant;
  return u;
}

