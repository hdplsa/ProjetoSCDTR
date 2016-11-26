#include "SketchMetaclass.h"

#define MASTER true
#define SLAVE false

//PUBLIC FUNTIONS
Meta::Meta(int ledPin,int sensorPin){
    //Inicialização do controlador PID
    this->_lightcontroller = new LightController(ledPin,sensorPin);
    this->_lightcontroller->setT(0.02);
    this->_lightcontroller->setRef(50);
    this->_lightcontroller->setSaturation(5);
    //Inicializaçoes do Modelo do sistema
    this->k11 = 0;
    this->k12 = 0;
    this->k21 = 0;
    this->k22 = 0;
    this->theta1 = 0;
    this->theta2 = 0;
}

void Meta::calibrateLumVoltageModel(){
   /*Indica o master da comunicação, necessário
    * no protocolo de calibração utilizado
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
