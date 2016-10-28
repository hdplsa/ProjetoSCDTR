#include "LightController.h"

LightController::LightController(int ledPin, int sensorPin){
  //Parâmetros do controlador
  this->Kp = 0.023;
  this->Ki = 0.0006;
  this->Kd = 0.000000;
  //Depêndencias do feedback
  this->ls = new LightSensor(sensorPin,5.0);
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
  this->sat_up = 5.0;
  this->sat_down = 0;

  // Windup
  this->Kw = 10;
  this->windup[0] = 0;
  this->windup[1] = 0;
}

//Inicialização - Calibração
void LightController::calibrateLumVoltage(){
  //Número de testes
  const int N = 10;
  //Variáveis experimentais
  double y[N];
  double u[N];
  double usquare[N];
  //Variáveis Regressão
  double sum = 0;
  double sumy = 0;
  double sumsquare = 0;
  double sumyu = 0;
  double det;
  //Recolha de dados para regressão linear
  for(int n=0;n<N;n++){
    u[n] = (Vcc/(double)N)*(double)n;
    this->ledp->setLedPWMVoltage(u[n]);
    delay(100);
    //Média de 10 observações
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

  //Saturação inferior (limite do modelo)
  this->sat_down = -this->teta/this->k;

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
  //Periodo de chamada da ação do controlador
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

void LightController::setSaturation(double sat_up){
  this->sat_up = sat_up;
}

double LightController::getK(){return this->k;}
double LightController::getTeta(){return this->teta;}

double LightController::getT(){
  return this->T;  
}

double LightController::getControlVariable(){
  return this->u[1];
}

/*
 * Calcula o sinal de FeedForward, é basicamente uma função
 * inversa do LED. Dá para cada Lux uma tensão a aplicar.
 */

double LightController::calcFeedForward(){
  
  double feedForward;

  feedForward = this->ref/this->k - this->teta;

  return feedForward;
  
}

/* Legenda dos sinais:
    e[1] -> sinal de erro no ciclo atual
    e[0] -> sinal de erro no ciclo -1

    u[1] -> sinal de controlo no ciclo atual
    u[0] -> sinal de controlo no ciclo -1
   
    ui_ant -> sinal de controlo do controlador integral do ciclo passado
    ud_ant -> sinal de controlo do controlador diferencial do ciclo anterior

    this->windup[1] -> corresponde ao sinal de anti-windup do ciclo atual
    this->windup[0] -> corresponde ao sinal de anti-windup do ciclo -1, 
      a ser utilizado no ciclo atual
    u_antes_sat -> corresponde ao sinal de entrada antes da saturação, que 
      é utilizado para calcular o anti-windup.

*/

double LightController::calcController(){
  double up, ui, ud, u_antes_sat, ff;
  
  // Avanço do tempo das samples dos sinais
  this->y = this->getSensorY();
  this->u[0] = this->u[1];
  this->e[0] = this->e[1];
  this->windup[0] = this->windup[1];
  
  // Cálculo do sinal de erro
  this->e[1] = this->calcErro();

  // Cálculo do sinal de controlo  
  up = this->calcPController();
  ui = this->calcPIController();
  ud = this->calcPDController();
  this->ui_ant = ui;
  this->ud_ant = ud;

  // Calcula o FeedForward
  ff = this->calcFeedForward();

  // Calcula o sinal de controlo final
  this->u[1] = up + ui + ud + ff;

  u_antes_sat = this->u[1];
  
  // Saturação na variável de controlo
  if (this->sat_up >= this->sat_down){
    if (this->u[1] < this->sat_down){
      this->u[1] = this->sat_down;
    }
    if (this->u[1] > this->sat_up){
      this->u[1] = this->sat_up;
    }
  }

  // Calcula o novo valor do windup
  this->windup[1] = this->u[1] - u_antes_sat;
  
  // debug serial
  Serial.print("y = ");
  Serial.println(this->y,4);
  Serial.print("e = ");
  Serial.println(e[1],4);
  Serial.print("u = ");
  Serial.println(u[1],4);
  return this->u[1];
}

void LightController::LEDInputControlVariable(){
  //Impõe sinal de comando calculado no LED
  this->ledp->setLedPWMVoltage(this->u[1]);
}

LightController::~LightController(){
  //Free à memória
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
  u = this->Ki*(this->T*0.5)*(this->e[1]+this->e[0]) + (this->T*0.5)*this->Kw*(this->windup[1]+this->windup[0]) + this->ui_ant;
  return u;
}

double LightController::calcPDController(){
  double u;
  //Controlo proporcional diferencial
  u = this->Kd*((2/this->T)*(this->e[1]-this->e[0])) - this->ud_ant;
  return u;
}

