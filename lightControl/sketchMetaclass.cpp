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
    const char _sread = "SR";
    double theta11, theta12;
    double theta21, theta22;
    double u[N];
    double y[N];
    /*Indica o master da comunicacao, necessario
     * no protocolo de calibracacao utilizado
     *
     * Por conveniencia, o Arduino Master na comunicacao
     * assume tera a Primeira linha das matrizes do modelo
     * [L] = [K]*[U] + [O], e o Slave a segunda linha
     */
    switch(this->defineFirst()){
        case FIRST:
            //---------------------------------------------------------------------
            for(int n;n<N;n++){
                u[n] = this->Setu(N,u[n],n);
                TWI::send_msg(1,_sread,strlen(_sread));
                y[n] = this->Gety();
            }
            //---------------------------------------------------------------------
            this->K11 = (this->MinSquare(N,u,y))[0];
            theta11 = (this->MinSquare(N,u,y))[1];
            sscanf(rI2C,"T=%4.1f",&theta12); //criar flag antes receber
            this->theta1 = (theta11 + theta12)*0.5;
            
            //--------------------------------------------------------------------
            //--------------------------------------------------------------------
            for(int n;n<N;n++){
                while(!strcmp(rI2C,_sread)){}
                y[n] = this->Gety();
            }
            K21 = (this->MinSquare(N,u,y))[0];
            theta21 = (this->MinSquare(N,u,y))[1];
            sprintf(_st21,"T=%4.1f",theta21); //criar flag antes de receber
            TWI::send_msg(1,_st21,strlen(_st21));
            
            
    }
    break;
    case SEC:
        //--------------------------------------------------------------------
        
        for(int n;n<N;n++){
            while(!strcmp(rI2C,_sread)){}
            y[n] = this->Gety();
        }
        K12 = (this->MinSquare(N,u,y))[0];
        theta12 = (this->MinSquare(N,u,y))[1];
        sprintf(_st12,"T=%4.1f",theta12); //criar flag antes de receber
        TWI::send_msg(1,_st12,strlen(_st12));
        
        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        
        for(int n;n<N;n++){
            u[n] = this->Setu(N,u[n],n);
            TWI::send_msg(1,_sread,strlen(_sread));
            y[n] = this->Gety();
        }
        //---------------------------------------------------------------------
        
        this->K22 = (this->MinSquare(N,u,y))[0];
        theta22 = (this->MinSquare(N,u,y))[1];
        sscanf(rI2C,"T=%4.1f",&theta22); //criar flag antes receber
        this->theta2 = (theta21 + theta22)*0.5;
        
        
        break;
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
    double sum = 0;
    double usquare[N];
    double sumsquare = 0;
    double sumy = 0;
    double sumyu = 0;
    double ans[2];
    double det = 0;
    double m = 0;
    double b = 0;
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
    return ans;
}

double Meta::Setu(const int N, double u, double PWM){
    u = (5.0/(double)N)*(double)PWM;
    this->_lightcontroller->ledp->setLedPWMVoltage(u); //isto é private, é preciso mudar
    delay(50);
    return u;
}
double Meta::Gety(){
    return this->_lightcontroller->ls->getAverageLuminousIntensity(N); //private shit
}

