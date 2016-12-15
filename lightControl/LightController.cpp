#include "LightController.h"

/*----------------------CONSTRUCTOR--------------------------------------*/
LightController::LightController(int Narduino,int ledPin, int sensorPin){

    //Numero de arduinos
    this->Narduino = Narduino;
    
    //Depêndencias do feedback
    this->ls = new LightSensor(sensorPin,5.0);
    this->ledp = new LedPWM(ledPin);
    
    //Alocação de vector de inputs
    this-> u = new double[Narduino];
    
    //Variáveis do modelo tensão/lux (Voltam a ser inicializadas no calibrate)
    this->k = 0;
    this->theta = 0;
}

/*----------------------PUBLIC FUNCTIONS---------------------------------*/
//SETFUNCTIONS
void LightController::SetIndex(int index){
    this->index = index;
}
void LightController::setRef(double ref){
    this->ref = (double)ref;
    this->ffflag = 1;
}
void LightController::setZeroUvec(){
  int i;
  for(i=0; i < this->Narduino; i++){
    this->u[i] = 0;
  }
}
void LightController::setUnFromdc(int dcn, int n){
  this->u[n] = dcn*(5.0/255.0);
}
void LightController::setLedU(double u){
    return this->ledp->setLedPWMVoltage(u);
}
void LightController::setSaturation(double sat_up, double sat_down){
    this->sat_up = sat_up;
    this->sat_down = sat_down;
}
void LightController::setK(double *k){
    this->k = k;
}
void LightController::setTheta(double theta){
    this->theta = theta;
}
//GET FUNCTIONS
double LightController::getRef(){
    return this->ref;
}
double LightController::getAverageY(const int N){
    return this->ls->getAverageLuminousIntensity(N);
}
int LightController::getOwnDutyCycle(){
    return (int)(this->u[this->index]*(255.0/5.0));
}

//CALCFUNCTIONS
double LightController::calcController(){
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
    double up, ui, ud, u_antes_sat, ff;
    
    if(this->ffflag == 1){
        // Primeiro ciclo só com feedforward
        this->u[this->index] = this->calcFeedForward();
        // Liga o PID
        this->ffflag = 0;
        // Coloca o sinal de comando no LED
        LEDInputControlVariable();
    } else {
        // Avanço do tempo das samples dos sinais
        this->y = this->getSensorY();
        
        this->u_ant = this->u[this->index];
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
        this->u_pid = up + ui + ud;
        this->u[this->index] = this->u_pid + ff;
        
        //Valor util no windup
        u_antes_sat = this->u[this->index];
        
        // Saturação na variável de controlo
        if (this->sat_up >= this->sat_down){
            if (this->u[this->index] < this->sat_down){
                this->u[this->index] = this->sat_down;
            }
            if (this->u[this->index] > this->sat_up){
                this->u[this->index] = this->sat_up;
            }
        }
        
        // Calcula o novo valor do windup
        this->windup[1] = this->u[this->index] - u_antes_sat;
        
        // Coloca o sinal de comando no LED
        LEDInputControlVariable();
    }
    
    return this->u[this->index];
}
//PRINTFUNCTION
double LightController::getY(){
    return this->y;
}
double LightController::getError(){
    return this->e[1];
}
double LightController::getOwnU(){
  return this->u[this->index];
}
//DECONSTRUCTOR
LightController::~LightController(){
    //Free à memória
    delete this->ls;
    delete this->ledp;
    delete this->u;
}
/*-----------------------------------------------------------------------*/
/*----------------------PRIVATE------------------------------------------*/
//GETFUNCTIONS
double LightController::getSensorY(){
    return ls->getLuminousIntensity();
}
//CALCFUNCTIONS
double LightController::calcErro(){
    //Cálcula erro de entrada no Controlador
    return this->ref-this->y;
}
double LightController::calcSumOtherKus(){
    int n, ans;
    for(n=0,ans=0; n < this->Narduino; n++){
        if(n != this->index){
            ans = ans + this->k[n]*this->u[n];
        }
    }
    return ans;
}
double LightController::calcFeedForward(){
    double feedforward;
    
    feedforward = (this->ref - this->calcSumOtherKus() - this->theta)/this->k[this->index];
    
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
//OTHER
void LightController::LEDInputControlVariable(){
    //Impõe sinal de comando calculado no LED
    this->ledp->setLedPWMVoltage(this->u[this->index]);
}
