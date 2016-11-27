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
 * é copiada para dentro da class para processamento
 * sempre que chamada receivedI2C
 */
void Meta::receivedI2C(char *str){
  strcpy(this->rI2C, str);
}

void Meta::calibrateLumVoltageModel(){
    double theta11, theta12;
    double theta21, theta22;
   /*Indica o master da comunicação, necessário
    * no protocolo de calibração utilizado
    * 
    * Por conveniencia, o Arduino Master na comunicação
    * assume terá a Primeira linha das matrizes do modelo
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
