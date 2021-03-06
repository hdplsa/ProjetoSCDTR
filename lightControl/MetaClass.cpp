#include "MetaClass.h"

//Estados de calibração
#define CHOICE  1
#define MASTER  2
#define SLAVE   3
#define SHUT 	4
#define TALK	5

#define OCCUPIEDLUM 50.0
#define NOTOCCUPIEDLUM 10.0

const double Umax = 5.0;

//PUBLIC FUNTIONS
Meta::Meta(int Narduino,int *ArduinoIndex,int ledPin,int sensorPin){
    int i;

    //Vector com indices dos Arduinos
    this->arduinoIndex = ArduinoIndex;
    this->Narduino = Narduino;
    //Inicializacao do controlador PID
    this->_lightcontroller = new LightController(Narduino,ledPin,sensorPin);
    this->_lightcontroller->setRef(OCCUPIEDLUM);
    this->_lightcontroller->setSaturation(5,0);
    //Init da string
    for(i = 0; i < 32; i++){
      this->rI2C[i] = '\0';
    }
    //Init do modelo feedforward
    this->k = new double[this->Narduino];
    this->initAllVector(this->k, this->Narduino);
    this->theta = 0;
    //Init vector de us
    this->initEnderecos();
    this->_lightcontroller->setZeroUvec();

    for(i=0; i < this->Narduino; i++){
      Error::setSerialString(this->arduinoIndex[i]);
    }
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
    int i,n;
    
    //STATE AND VARIABLE INIT
    this->setLedU(0);
    int STATE = CHOICE;
    for(n = 0; n < dimU; n++){
        u[n] = n*(Umax/dimU);
    }

    Serial.println("START STATE MACHINE");
    for(i=0; i < this->Narduino; i++){
        //choice of whos MASTER or SLAVE        
        if(this->arduinoIndex[i] == EEPROM.read(0)){
            Serial.println("MASTER");
            Error::setSerialString("MASTER");
            STATE = MASTER;
        }else{
            Serial.println("SLAVE");
            Error::setSerialString("SLAVE");
            STATE = SLAVE;
        }
        //-----------------------------------STATE MACHINE START---------------------------------------
        switch(STATE){
        //-----------------------------
        case MASTER:
            delay(200);
            for(n = 0; n < dimU; n++){
                 delay(20);
                //Valor de entrada no LED
                this->setLedU(u[n]);
                //Inicializa a string de novo
                this->resetI2CString();;
                //Global call para todos lerem y
                TWI::send_msg(0,"SR",strlen("SR"));
                //Esperar que os restantes leiam
                while(!this->sendflag){};
                this->sendflag = false;
                //Leitura do próprio sensor
                y[n] = this->Gety(N);
                Serial.println(y[n],4);
            }
            this->setLedU(0); // lightoff
            //Global call para todos fazer LeastSquare
            TWI::send_msg(0,"MS",strlen("MS"));
            //Esperar que os restantes leiam
            while(!this->sendflag){};
            this->sendflag = false;
            //Determinar k_j, theta_j
            ms = this->LeastSquare(dimU, u, y);
            this->k[i] = ms[0];
            theta_[i] = ms[1];
            delete ms;
            //Esperar pelos calculos
        break;
        //-----------------------------
        case SLAVE:
            //Caso de ser SLAVE
            n = 0; bool slaveEnd = false;
            while(!slaveEnd){
                //Espera comando do MASTER
                while(!this->recvflag){};
                this->recvflag = false;
                if((this->rI2C[0] == 'S')&&(this->rI2C[1] == 'R')){
                    //Inicializa a string de novo
                    this->resetI2CString();
                    //Leitura do próprio sensor
                    y[n] = this->Gety(N);
                    Serial.println(y[n],4);
                    n++;
                }
                if((this->rI2C[0] == 'M')&&(this->rI2C[1] == 'S')){
                    //Inicializa a string de novo
                    this->resetI2CString();;
                    //Determinar k_j, theta_j
                    ms = this->LeastSquare(dimU, u, y);
                    this->k[i] = ms[0];
                    theta_[i] = ms[1];
                    delete ms;
                    slaveEnd = true;
                }
            }
        break;
        //-----------------------------
        }
    }
    //-----------------------------------STATE MACHINE END-----------------------------------------
    //Valor final do offset, theta do modelo
    this->theta = this->calcVectorAverage(theta_, this->Narduino);
    //Meter os valores dentro da classe do controlador (LightController)
    this->_lightcontroller->setK(k);
    this->_lightcontroller->setTheta(theta);
}

//Imprime matriz [K] e [Theta] do modelo
void Meta::printModel(){
  int i;
  Serial.print("model: [");
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
  int STATE, i, dc;
  char send[32];
  
  for(i=0; i < this->Narduino; i++){
    //choice of whos MASTER or SLAVE        
    if(this->arduinoIndex[i] == EEPROM.read(0)){
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
            //Set dutycycle
            _lightcontroller->setUnFromdc(dc, i);
        }
      break;
      //-----------------------------
    }
  }
}

bool Meta::getOccupancy(){
  return this->o;
}

double Meta::getTheta(){
    return this->theta;
}

void Meta::setOccupancy(int o){
  this->o = o;
  //Coloca referencia em função do estado de ocupação
  if (this->o){
    this->_lightcontroller->setRef(OCCUPIEDLUM);
  } else {
    this->_lightcontroller->setRef(NOTOCCUPIEDLUM);
  }
}

Meta::~Meta(){
    //Frees do programa no final
    delete this->k;
    delete this->_lightcontroller;
}

/* Associa o valor que está na EEPROM do Arduino
 * e consequente ordem na comunicação ao indice
 * no vector do controlador */
void Meta::initEnderecos(){
  int i;
  //Percorre o vector até encontrar o indice com o valor da EEPROM
  for(i=0; i < this->Narduino; i++){
      //Verifica se é o Arduino na posição actual
      if(arduinoIndex[i] == EEPROM.read(0)){
          //Coloca valor do indice no controlador
          this->_lightcontroller->SetIndex(i);
      }
  }
}

char *Meta::strAlloc(int len){
  return new char[len+1];
}

void Meta::initAllVector(double *v, int dim){
    int i;
    for(i=0; i < dim; i++){
        v[i] = 0;
    }
}

//Calcula a media de um vector v de dimensão dim de doubles
double Meta::calcVectorAverage(double *v, int dim){
    int i;
    double av;

    for(i=0, av=0; i < dim; i++){
        av = av + v[i];
    }
    av = av/dim;
    return av;
}

//Inicializa string I2C com string vazia
void Meta::resetI2CString(){
    this->rI2C[0] = '\0';
}

double *Meta::LeastSquare(const int N, double *u, double *y){
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
    //cria variável de retorno
    ans = new double[2];
    ans[0] = m;
    ans[1] = b;
    return ans;
}

void Meta::setLedU(double u){
  this->_lightcontroller->setLedU(u);
  delay(200);
}


double Meta::Gety(const int N){
    return this->_lightcontroller->getAverageY(N);
}
