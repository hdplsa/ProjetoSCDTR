#include "LightController.h"

LightController::LightController(int ledPin, int sensorPin){
    
    //Depêndencias do feedback
    this->ls = new LightSensor(sensorPin,5.0);
    this->ledp = new LedPWM(ledPin);
    
    //Variáveis do modelo tensão/lux (Voltam a ser inicializadas no calibrate)
    this->k = 0;
    this->teta = 0;
}

// //Inicialização - Calibração
// void LightController::calibrateLumVoltage(){
//   //Número de testes
//   const int N = 10;
//   //Variáveis experimentais
//   double y[N];
//   double u[N];
//   double usquare[N];
//   //Variáveis Regressão
//   double sum = 0;
//   double sumy = 0;
//   double sumsquare = 0;
//   double sumyu = 0;
//   double det;
//   //Recolha de dados para regressão linear
//   for(int n=0;n<N;n++){
//     u[n] = (Vcc/(double)N)*(double)n;
//     this->ledp->setLedPWMVoltage(u[n]);
//     delay(50);
//     //Média de 10 observações
//     y[n] = this->ls->getAverageLuminousIntensity(10);
//     //Prints de debug
//     /*Serial.print(n);
//     Serial.print(' ');
//     Serial.print(y[n]);
//     Serial.print('\n');
//     Serial.println("------------------------------------------");*/
//   }
//
//   //Regressão Linear (u - entrada, y - saída) mínimos quadrados
//   for(int n=0;n<N;n++){
//     sum+=u[n];
//     usquare[n]=u[n]*u[n];
//     sumsquare+=usquare[n];
//     sumy+=y[n];
//     sumyu+=y[n]*u[n];
//   }
//   //Modelo matemático l = k*u+teta
//   det = 1/(N*sumsquare - sum*sum);
//   this->k = det*(N*sumyu - sum*sumy);
//   this->teta = det*(-sum*sumyu + sumsquare*sumy);
//
//   //Desligar a luz no fim
//   this->lightoff();
//
//   //Saturação inferior (limite do modelo)
//   this->sat_down = -this->teta/this->k;
//
//   //Esperar um pouco para estar ready
//   delay(10);
//
//   //Debug Stuff. descomentar quando necessário
//   Serial.print("K = ");
//   Serial.print(this->k);
//   Serial.print('\n');
//   Serial.print("theta = ");
//   Serial.print(this->teta);
//   Serial.print("\n\n");
// }

void LightController::lightoff(){
  this->ledp->setLedPWMVoltage(0);
}

void LightController::lighton(){
  this->ledp->setLedPWMVoltage(5);
}

void LightController::setT(double T){
    //Periodo de chamada da accao do controlador
    this->T = T;
}

void LightController::setY(double y){
    this->y = y;
}

void LightController::setRef(int ref){
    this->ref = (double)ref;
    this->ffflag = 1;
}

void LightController::setU(double u){
    this->u[1] = u;
}

void LightController::setSaturation(double sat_up){
    this->sat_up = sat_up;
}

// Retorna o parametro K da plant (LED)
double LightController::getK(){
    return this->k;
}

// Retorna o parâmetro Teta da plant (LED)
double LightController::getTeta(){
    return this->teta;
}

// Retorna o periodo
double LightController::getT(){
    return this->T;
}

// Retorna o sinal de controlo a ser aplicado ao LED
double LightController::getControlVariable(){
    return this->u[1];
}

// Retorna o y guardado no objeto
double LightController::getY(){
    return this->y;
}


// Retorna o erro do ciclo "atual"
double LightController::getError(){
    return this->e[1];
}

/* Legenda dos sinais:
 * e[1] -> sinal de erro no ciclo atual
 * e[0] -> sinal de erro no ciclo -1
 *
 * u[1] -> sinal de controlo no ciclo atual
 * u[0] -> sinal de controlo no ciclo -1
 *
 * ui_ant -> sinal de controlo do controlador integral do ciclo passado
 * ud_ant -> sinal de controlo do controlador diferencial do ciclo anterior
 *
 * this->windup[1] -> corresponde ao sinal de anti-windup do ciclo atual
 * this->windup[0] -> corresponde ao sinal de anti-windup do ciclo -1,
 * a ser utilizado no ciclo atual
 * u_antes_sat -> corresponde ao sinal de entrada antes da saturação, que
 * é utilizado para calcular o anti-windup.
 */

double LightController::calcController(){
    double up, ui, ud, u_antes_sat, ff;
    
    if(this->ffflag == 1){
        // Primeiro ciclo só com feedforward
        this->u[1] = this->calcFeedForward();
        // Liga o PID
        this->ffflag = 0;
    } else {
        // Avanço do tempo das samples dos sinais
        this->y = this->getSensorY();
        
        this->u[0] = this->u[1];
        this->e[0] = this->e[1];
        this->windup[0] = this->windup[1];
        
        // Cálculo do sinal de erro
        this->e[1] = this->calcDeadzone(this->calcErro());
        
        // Cálcula o feedforward
        ff = this->calcFeedForward();
        
        // Cálculo dos sinais de controlo
        up = this->calcPController();
        ui = this->calcPIController();
        ud = this->calcPDController();
        this->ui_ant = ui;
        this->ud_ant = ud;
        
        // Gera o sinal de controlo
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
        
        // Coloca o sinal de comando no LED
        LEDInputControlVariable();
    }
    
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
    return ls->getLuminousIntensity();
}

double LightController::calcErro(){
    //Cálcula erro de entrada no Controlador
    return this->ref-this->y;
}

double LightController::calcFeedForward(){
    double feedforward;
    
    feedforward = (this->ref - this->teta)/this->k;
    
    return feedforward;
}

double LightController::calcDeadzone(double e){
    
    double error = e;
    double deadzone;
    
    deadzone = 0.0355*this->ref - 0.5582;
    
    if(e > -deadzone && e < 0){
        error = 0;
    } else {
      if(e < deadzone && e > 0) {
          error = 0;
      }
    }
    return error;
    
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
