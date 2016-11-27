#include "SketchMetaclass.h"

#define FIRST true
#define SEC false

//PUBLIC FUNTIONS
Meta::Meta(double T,int ledPin,int sensorPin){
    //Inicializacao do controlador PID
    this->_lightcontroller = new LightController(ledPin,sensorPin);
    this->_lightcontroller->setT(T);
    this->_lightcontroller->setRef(50);
    this->_lightcontroller->setSaturation(5);
}

LightController *Meta::getController(){
    return this->_lightcontroller;
}

/*String recebida assincronamente pelo protocolo I2C
 * e copiada para dentro da class para processamento
 * sempre que chamada receivedI2C
 */
void Meta::receivedI2C(char *str){
    strcpy(this->rI2C, str);
}

void Meta::calibrateLumVoltageModel(){
    const int N = 10;
    const char _sread = "SENSORREAD";
    double theta11, theta12;
    double theta21, theta22;
    double u[N];
    /*Indica o master da comunicacao, necessario
     * no protocolo de calibracacao utilizado
     *
     * Por conveniencia, o Arduino Master na comunicacao
     * assume tera a Primeira linha das matrizes do modelo
     * [L] = [K]*[U] + [O], e o Slave a segunda linha
     */
    switch(this->defineMaster()){
        case FIRST:
            for(int n;n<N,n++){
                this->setu(this->_lightcontroller,N,u[n],n);
                TWI::send_msg(1,_sread,strlen(_sread));
            }
            break;
        case SEC:
            
            break;
    }
}

Meta::~Meta(){
    delete this->_lightcontroller;
}

//PRIVATE FUNCTIONS
bool Meta::defineFirst(){
    if (EEPROM.read(0) == 0)
        return true;
    else
        return false;
}

double *Meta::MinSquare(const int N, double *u, double *y){
    double ans[2];
    for(int n=0;n<N;n++){
        sum+=u[n];
        usquare[n]=u[n]*u[n];
        sumsquare+=usquare[n];
        sumy+=y[n];
        sumyu+=y[n]*u[n];
    }
    //Modelo matematico y = m*u+b
    det = 1/(N*sumsquare - sum*sum);
    m = det*(N*sumyu - sum*sumy);
    b = det*(-sum*sumyu + sumsquare*sumy);
    
    ans[0] = m;
    ans[1] = b;
    return ans
}

void Meta::Setu(LightController *_lightcontroller,const int N, double u, double PWM){
    u = ((this->_lightcontroller->Vcc)/(double)N)*(double)PWM;
    this->lightcontroller->ledp->setLedPWMVoltage(u);
    delay(50);
}
double Meta::Gety(){
    return this->_lightcontroller->ls_getAverageLuminousIntensity(N);
}

