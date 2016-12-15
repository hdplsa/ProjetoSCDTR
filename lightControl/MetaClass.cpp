#include "MetaClass.h"

//Estados de calibração
#define CHOICE  1
#define MASTER  2
#define SLAVE   3
#define SHUT 	4
#define TALK	5


const double Umax = 5.0;

//PUBLIC FUNTIONS
Meta::Meta(int Narduino,int ledPin,int sensorPin){
    int i;
    //Inicializacao do controlador PID
    this->_lightcontroller = new LightController(Narduino,ledPin,sensorPin);
    this->_lightcontroller->setRef(50.0);
    this->_lightcontroller->setSaturation(5,0);
    //Init da string
    for(i = 0; i < 32; i++){
      this->rI2C[i] = '\0';
    }
    //Init do modelo feedforward
    this->Narduino = Narduino;
    this->k = new double[this->Narduino];
    for(i = 0; i < this->Narduino; i++){
      this->k[i] = 0;
    }
    this->theta = 0;
    //Init vector de us
    this->initEnderecos();
    this->_lightcontroller->setZeroUvec();
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
    const int dimU = 5; //dimensao do vector de tensoes
    double theta_[this->Narduino]; //numero de thetas intermedios (Num. de Arduinos)
    double u[dimU], y[dimU]; //vector de entrada (tensao) e de saida (luminancia)
    double *ms; //vector de resposta de minimos quadrados
    int j,n;
    
    //STATE AND VARIABLE INIT
    this->Setu(0);
    int STATE = CHOICE;
    for(n = 0; n < dimU; n++){
        u[n] = n*(Umax/dimU);
    }

    Serial.println("START STATE MACHINE");
    for(j=10; j < 10+this->Narduino; j++){
        //choice of whos MASTER or SLAVE        
        if(j == EEPROM.read(0)){
            Serial.println("MASTER");
            STATE = MASTER;
        }else{
            Serial.println("SLAVE");
            STATE = SLAVE;
        }
        //-----------------------------------STATE MACHINE START---------------------------------------
        switch(STATE){
        //-----------------------------
        case MASTER:
        //Serial.println("AM MASTER");
            delay(1000);
            for(n = 0; n < dimU; n++){
                 delay(20);
                //Valor de entrada no LED
                this->Setu(u[n]);
                Serial.println("CHANGED u");
                this->rI2C[0] = '\0';
                //Global call para todos lerem y
                TWI::send_msg(0,"SR",strlen("SR"));
                //Esperar que os restantes leiam
                //Serial.println("WAITING TO SR");
                while(!this->sendflag){};
                //Serial.println("SEND SUCCESSFULL");
                this->sendflag = false;
                //Leitura do próprio sensor
                y[n] = this->Gety(N);
                Serial.println(y[n],4);
            }
            this->Setu(0); // lightoff
            //Global call para todos fazer minSquare
            TWI::send_msg(0,"MS\0",strlen("MS")+1);
            //Esperar que os restantes leiam
            //Serial.println("WAITING TO MS");
            while(!this->sendflag){};
            //Serial.println("SEND SUCCESSFULL");
            this->sendflag = false;
            //Determinar k_j, theta_j
            ms = this->MinSquare(dimU, u, y);
            this->k[j-10] = ms[0];
            theta_[j-10] = ms[1];
            delete ms;
            //Esperar pelos calculos
        break;
        //-----------------------------
        case SLAVE:
        //Serial.println("AM SLAVE");
            //Caso de ser SLAVE
            n = 0; bool slaveEnd = false;
            while(!slaveEnd){
                //Espera comando do MASTER
                //Serial.println("WAITING FOR SR");
                while(!this->recvflag){};
                //Serial.println("SR RECEIVED");
                this->recvflag = false;
                if((this->rI2C[0] == 'S')&&(this->rI2C[1] == 'R')){
                    //Serial.print("SR = ");
                    this->rI2C[0] = '\0';
                    //Leitura do próprio sensor
                    y[n] = this->Gety(N);
                    Serial.println(y[n],4);
                    n++;
                }
                if((this->rI2C[0] == 'M')&&(this->rI2C[1] == 'S')){
                    //Serial.println("MS ==");
                    this->rI2C[0] = '\0';
                    //Determinar k_j, theta_j
                    ms = this->MinSquare(dimU, u, y);
                    this->k[j-10] = ms[0];
                    theta_[j-10] = ms[1];
                    delete ms;
                    slaveEnd = true;
                }
            }
        break;
        //-----------------------------
        }
    }
    //-----------------------------------STATE MACHINE END-----------------------------------------
    //Valor final do offset
    for(j=0; j < this->Narduino; j++){
        this->theta = this->theta + theta_[j];
    }
    this->theta = this->theta/this->Narduino;

    //Meter os valores dentro da classe do controlador (LightController)
    this->_lightcontroller->setK(k);
    this->_lightcontroller->setTheta(theta);
}


void Meta::printModel(){
  int i;
  Serial.print("[");
  for(i=0; i < this->Narduino; i++){
    Serial.print(this->k[i],4);
    Serial.print(" ");
  }
  Serial.print("] ");
  Serial.println(this->theta,4);
}

void Meta::setSendFlag(bool sendflag){
  this->sendflag = sendflag;
}

bool Meta::getSendFlag(){
  return this->sendflag;
}

void Meta::setu_vec(){
  int STATE, j, dc;
    char send[32];
  
  for(j=10; j < 10+this->Narduino; j++){
        //choice of whos MASTER or SLAVE        
        if(j == EEPROM.read(0)){
            this->_lightcontroller->SetIndex(j-10);
            Serial.println("MASTER");
            STATE = TALK;
        }else{
            Serial.println("SLAVE");
            STATE = SHUT;
        }
    switch(STATE){
      //-----------------------------
      case TALK:
                //Espera antes de falar
                delay(20);
        //Global call para enviar U
        TWI::send_msg(0,"U",strlen("U"));
        //Esperar mensagem enviada
        while(!this->sendflag){};
        this->sendflag = false;
                //Valor de entrada no LED
        Serial.println("Sent U");
                //Get dutycycle
                dc = _lightcontroller->getOwnDutyCycle();
                //Envia valor
                sprintf(send,"%d",dc);
                //Espera antes de falar
                delay(20);
                //Global call para enviar valor de U
                TWI::send_msg(0,send,strlen(send));
                //Esperar mensagem enviada
        while(!this->sendflag){};
        this->sendflag = false;
      break;
            //-----------------------------
      case SHUT:
        while(!this->recvflag){};
                this->recvflag = false;
                //Esperar por U para receber valor
                if(this->rI2C[0] == 'U'){
                    while(!this->recvflag){};
                    this->recvflag = false;
                    //Obter duty cycle da mensagem
                    sscanf(this->rI2C,"%d",&dc);
                    Serial.print("dutycycle = ");
                    Serial.println(dc);
                    //Set dutycycle
                    _lightcontroller->setUnFromdc(dc, j-10);
                }
      break;
      //-----------------------------
    }
  }
}

bool Meta::getOccupancy(){
  return this->o;
}

void Meta::setOccupancy(int o){
  this->o = o;
  //Coloca referencia em função do estado de ocupação
  if (this->o){
    this->_lightcontroller->setRef(50.0);
  } else {
    this->_lightcontroller->setRef(10.0);
  }
}

Meta::~Meta(){
    delete this->_lightcontroller;
}

void Meta::initEnderecos(){
  double valor;
  valor = EEPROM.read(0);
  this->_lightcontroller->SetIndex(valor-10);
}

char *strAlloc(int len){
  return new char[len+1];
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

void Meta::Setu(double u){
  this->_lightcontroller->setLedU(u);
  delay(50);
}


double Meta::Gety(const int N){
    return this->_lightcontroller->getAverageY(N);
}
