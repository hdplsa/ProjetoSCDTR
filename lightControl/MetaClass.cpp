#include "MetaClass.h"

//Estados de calibração
#define CHOICE  1
#define MASTER  2
#define SLAVE   3


const double Umax = 5.0;

//PUBLIC FUNTIONS
Meta::Meta(int Narduino,double T,int ledPin,int sensorPin){
    int i;
    //Inicializacao do controlador PID
    this->_lightcontroller = new LightController(Narduino,ledPin,sensorPin);
    this->_lightcontroller->setT(T);
    this->_lightcontroller->setRef(50);
    this->_lightcontroller->setSaturation(5,0);

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

    //-----------------------------------STATE MACHINE START---------------------------------------
    for(j=10; j < 10+this->Narduino; j++){
        Serial.println("MUDEI DE ARDUINO MASTER------------");
        delay(2000);
        switch(STATE){
            //-----------------------------
            case CHOICE:
                if(j == EEPROM.read(0)){
                    this->_lightcontroller->SetIndex(j);
                    STATE = MASTER;
                }else{
                    STATE = SLAVE;
                }
            break;
            //-----------------------------
            case MASTER:
                for(n = 0; n < dimU; n++){
                    //Valor de entrada no LED
                    this->Setu(u[n]);
                    Serial.println("MUDEI u");
                    //Global call para todos lerem y
                    TWI::send_msg(0,"SR",strlen("SR"));
                    //Esperar que os restantes leiam
                    while(!this->sendflag){};
                    this->sendflag = false;
                    //Leitura do próprio sensor
                    y[n] = this->Gety(N);
                    delay(10);
                }
                this->Setu(0); // lightoff
                //Global call para todos fazer minSquare
                TWI::send_msg(0,"MS",strlen("MS"));
                //Esperar que os restantes leiam
                while(!this->sendflag){};
                this->sendflag = false;
                //Determinar k_j, theta_j
                ms = this->MinSquare(N, u, y);
                this->k[j-10] = ms[0];
                theta_[j-10] = ms[1];
                delete ms;
                //Esperar pelos calculos
                delay(20);
                
                STATE = CHOICE;
            break;
            //-----------------------------
            case SLAVE:
                //Caso de ser SLAVE
                for(n = 0; n < dimU; n++){
                    //Esperar pelo "SR"
                    while(!this->recvflag){};
                    this->recvflag = false;
                    //se recebeu a mensagem "SR"
                    if((this->rI2C[0] == 'S')&&(this->rI2C[1] == 'R')){
                        Serial.println("SR ==");
                        //Leitura do próprio sensor
                        y[n] = this->Gety(N);
                    }
                }
                //Esperar pelo "MS"
                while(!this->recvflag){};
                this->recvflag = false;
                //se recebeu a mensagem "MS"
                if((this->rI2C[0] == 'M')&&(this->rI2C[1] == 'S')){
                    Serial.println("MS ==");
                    //Determinar k_j, theta_j
                    ms = this->MinSquare(N, u, y);
                    this->k[j-10] = ms[0];
                    theta_[j-10] = ms[1];
                    delete ms;

                    STATE = CHOICE;
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

Meta::~Meta(){
    delete this->_lightcontroller;
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
  this->_lightcontroller->_Setu(u);
  delay(50);
}

double Meta::Gety(const int N){
    return this->_lightcontroller->getAverageY(N);
}

