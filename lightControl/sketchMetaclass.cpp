#include "SketchMetaclass.h"

#define MASTER true
#define SLAVE false

//PUBLIC FUNTIONS
Meta::Meta(double T,int ledPin,int sensorPin){
    //Inicialização do controlador PID
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
    double theta11, theta12;
    double theta21, theta22;
    double u[N];
    double *aux;
   /*Indica o master da comunicacao, necessario
    * no protocolo de calibracacao utilizado
    * 
    * Por conveniencia, o Arduino Master na comunicacao
    * assume tera a Primeira linha das matrizes do modelo
    * [L] = [K]*[U] + [O], e o Slave a segunda linha
    */
   switch(this->defineMaster()){
      case MASTER:
        
        break;
      case SLAVE:
      
        break;
   }
}

Meta::~Meta(){
    delete this->_lightcontroller;
}

//PRIVATE FUNCTIONS 
bool Meta::defineMaster(){
    if (EEPROM.read(0) == 0)
        return true;
    else
        return false;
}

double *Meta::calibrateLumVoltage(LightController *_lightcontroller,int N,double *u){
    //Variavel return
    double ans[2];
    //variaveis auxiliares
    double k;
    double teta;
    //Variaveis experimentais
    double y[N];
    double usquare[N];
    //Variaveis Regressao
    double sum = 0;
    double sumy = 0;
    double sumsquare = 0;
    double sumyu = 0;
    double det;
    //Recolha de dados para regressaoo linear
    for(int n=0;n<N;n++){
        u[n] = (Vcc/(double)N)*(double)n;
        this->_lightcontroller->ledp->setLedPWMVoltage(u[n]);
        delay(50);
        //Media de 10 observacoes
        y[n] = this->_lightcontroller->ls->getAverageLuminousIntensity(10);
    }
    //Regressao Linear (u - entrada, y - saida) minimos quadrados
    for(int n=0;n<N;n++){
        sum+=u[n];
        usquare[n]=u[n]*u[n];
        sumsquare+=usquare[n];
        sumy+=y[n];
        sumyu+=y[n]*u[n];
    }
    //Modelo matematico l = k*u+teta
    det = 1/(N*sumsquare - sum*sum);
    k = det*(N*sumyu - sum*sumy);
    teta = det*(-sum*sumyu + sumsquare*sumy);
    
    //Desligar a luz no fim
    this->-lightcontroller->lightoff();
    
    /*//Saturacao inferior (limite do modelo)
     * this->sat_down = -this->teta/this->k;
     *
     * //Esperar um pouco para estar ready*/
    delay(10);
    
    //retornar valores dos m�nimos quadrados
    ans[0] = k;
    ans[1] = teta;
    return ans            
}