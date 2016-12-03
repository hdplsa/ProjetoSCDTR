#include "SketchMetaClass.h"

//Estados de calibração
#define INIT    1
#define SELFL   2
#define OTHERL  3
#define RECIEVE 4
//Endereços I2C
#define I2CAddFir 10
#define I2CAddSec 11

const double Umax = 5.0;

//PUBLIC FUNTIONS
Meta::Meta(int Narduino,double T,int ledPin,int sensorPin){
    int i;
    //Inicializacao do controlador PID
    this->_lightcontroller = new LightController(ledPin,sensorPin);
    this->_lightcontroller->setT(T);
    this->_lightcontroller->setRef(50);
    this->_lightcontroller->setSaturation(5);

    for(i = 0; i < 20; i++){
      this->rI2C[i] = '\0';
    }
    //Init do modelo feedforward
    this->Narduino = Narduino;
    for(i = 0; i < this->Narduino; i++){
      this->k[i] = 0;
    }
    this->theta = 0;
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
    this->recvflag = true;
}

void Meta::calibrateLumVoltageModel(){
  const int N = 10; //Numero de leituras por teste
  const int Udim = 5;
  double theta_[this->Narduino];
  double u[Udim], y[Udim];
  double *ms;
  int j,n;
  //Valores de input no LED
  for(n = 0; n < Udim; n++){
    u[n] = n*(Umax/Udim);
  }
  //Coluna j da matriz [k]
  for(j=10; j < 10+this->Narduino; j++){
    Serial.println("MUDEI DE ARDUINO MASTER------------");
    delay(2000);
    //Define master actual
    if(j == EEPROM.read(0)){
      //Caso de ser MASTER
      for(n = 0; n < Udim; n++){
        //Valor de entrada no LED
        this->setu2(u[n]);
        Serial.println("MUDEI u");
        //Global call para todos lerem y
        TWI::send_msg(0,"SR",strlen("SR"));
        //Esperar que os restantes leiam
        while(!this->sendflag){};
        this->sendflag = true;
        //Leitura do próprio sensor
        y[n] = this->Gety(N);
        delay(10);
      }
      //Global call para todos lerem y
      TWI::send_msg(0,"MS",strlen("MS"));
      //Esperar que os restantes leiam
      while(!this->sendflag){};
      this->sendflag = true;
      //Determinar k_j, theta_j
      ms = this->MinSquare(N, u, y);
      this->k[j] = ms[0];
      theta_[j] = ms[1];
      delete ms;
      //Esperar pelos calculos
      delay(20);
    } else {
      //Caso de ser SLAVE
      for(n = 0; n < Udim; n++){
        //Esperar pelo "SR"
        while(!this->recvflag){};
        this->recvflag = true;
        if(!strcmp(this->rI2C,"SR")){
          Serial.println("SR ==");
          //Leitura do próprio sensor
          y[n] = this->Gety(N);       
        } 
      }
      //Esperar pelo "MS"
      while(!this->recvflag){};
      this->recvflag = true;
      if(!strcmp(this->rI2C,"MS")){
        Serial.println("MS ==");
        //Determinar k_j, theta_j
        ms = this->MinSquare(N, u, y);
        this->k[j] = ms[0];
        theta_[j] = ms[1]; 
        delete ms;
      }
    }   
  }
  //Valor final do offset
  for(j=0; j < this->Narduino; j++){
    this->theta = this->theta + theta_[j];
  }
  this->theta = this->theta/this->Narduino;
}

void Meta::printModel(){
  int i;
  Serial.print("[");
  for(i=0; i < this->Narduino; i++){
    Serial.print(this->k[i],4);
  }
  Serial.print("] ");
  Serial.println(this->theta,4);
}

void Meta::calibrateLumVoltageModel_old(){
    const int N = 10;
    double u[N];
    double y[N];
    double theta11, theta12;
    double theta21, theta22;
    char _sread[] = "SR";
    char _done[]  = "DN";
    char _t11[20];
    char _t12[20];
    char _t21[20];
    char _t22[20];

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

bool Meta::getSendFlag(){
  return this->sendflag;
}

Meta::~Meta(){
    delete this->_lightcontroller;
}

char *strAlloc(int len){
  return new char[len+1];
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
    double *ans;
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

    ans = new double[2];
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

void Meta::setu2(double u){
  this->_lightcontroller->_Setu(u);
  delay(50);
}

double Meta::Gety(const int N){
    return this->_lightcontroller->getAverageY(N);
}

