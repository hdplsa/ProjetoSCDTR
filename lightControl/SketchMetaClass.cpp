#include "SketchMetaClass.h"

//Estados de calibração
#define INIT    1
#define SELFL   2
#define OTHERL  3
#define RECIEVE 4
//Endereços I2C
#define I2CAddFir 10
#define I2CAddSec 11

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
    double u[N];
    double y[N];
    double theta11, theta12;
    double theta21, theta22;
    const char _sread = "SR";
    const char _done  = "DN";
    char *_t11;
    char *_t12;
    char *_t21;
    char *_t22;

    int STATE = INIT;
    int n = 0;
    bool END = false;

    int Addr = 0;

    while(!END){
      switch(STATE){
        //--------------------------------
        case INIT:
          Serial.println("INIT");
          if(this->First()){
            Addr = I2CAddSec;
            STATE = SELFL;
          }else{
            Addr = I2CAddFir;
            STATE = OTHERL; 
          }
          Serial.println(STATE);
        break;
        //--------------------------------
        case SELFL:
          Serial.println("SELFL");
          u[n] = Setu(N,u[n],n);
          TWI::send_msg(Addr,_sread,strlen(_sread));
          y[n] = Gety(N);
          //while(!sendflag){};
          //TWI::set_slaveR();
          while(!strcmp(this->rI2C,_done)){}
          n++;
          if(n>=N){
            if(this->First()){
              this->K11 = (this->MinSquare(N,u,y))[0];
              theta11 = (this->MinSquare(N,u,y))[1]; 
              STATE = RECIEVE;
            }else{
              this->K22 = (this->MinSquare(N,u,y))[0];
              theta22 = (this->MinSquare(N,u,y))[1]; 
              STATE = RECIEVE;
            }
             Serial.print("u = ");
             Serial.println(u[n],4);
            n=0;
          }
        break;
        //--------------------------------
        case OTHERL:
          Serial.println("OTHERL");
          while(!strcmp(rI2C,_sread)){} //NEED SYNCHRNOUS SO IT READS THE RIGHT PWM SET OF THE OTHER MACHINE.
          y[n] = this->Gety(N);
          TWI::send_msg(Addr,_done,strlen(_done));
          n++;
          if(n>=N){
            if(this->First()){
              this->K12 = (this->MinSquare(N,u,y))[0];
              theta21 = (this->MinSquare(N,u,y))[1]; 
              sprintf(_t21,"T=%4.1f",theta21);
              TWI::send_msg(Addr,_t21,strlen(_t21));
              END = true;
            }else{
              this->K21 = (this->MinSquare(N,u,y))[0];
              theta12 = (this->MinSquare(N,u,y))[1];
              sprintf(_t12,"T=%4.1f",theta12);
              TWI::send_msg(Addr,_t12,strlen(_t12));
              STATE = SELFL;
            }
            n=0;
          }
        break;
        //--------------------------------
        case RECIEVE:
          Serial.println("RECEIVE");
          TWI::set_slaveR();
          while((!strcmp(this->rI2C,_t12)) || (!strcmp(this->rI2C,_t21))){}
          if(this->First() /* && Recebeu Algo no rI2C*/){
            sscanf(this->rI2C,"T=%4.1f",&theta12);
            this->theta1 = (theta11 + theta12)*0.5;
            STATE = OTHERL;
          }else{
            sscanf(this->rI2C,"T=%4.1f",&theta21);
            this->theta2 = (theta21 + theta22)*0.5;
            END = true;
          }
          Serial.print("string: ");
          Serial.println(this->rI2C);
        break;
        //--------------------------------
        }
    }
}

 void Meta::setSendFlag(bool sendflag){
  this->sendflag = sendflag;
 }

Meta::~Meta(){
    delete this->_lightcontroller;
}

//PRIVATE FUNCTIONS
bool Meta::First(){
    if (EEPROM.read(0) == 10)
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
    this->_lightcontroller->_Setu(u);
    delay(50);
    return u;
}
double Meta::Gety(const int N){
    return this->_lightcontroller->getAverageY(N);
}

