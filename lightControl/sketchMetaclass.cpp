#include "SketchMetaclass.h"

//PUBLIC FUNTIONS
Meta::Meta(int ledPin,int sensorPin){
    this->_lightcontroller = new LightController(ledPin,sensorPin);
    this->_lightcontroller->setT(0.02);
    this->_lightcontroller->setRef(50);
    this->_lightcontroller->setSaturation(5);
}

void Meta::calibrateLumVoltage(){
   
}

Meta::~Meta(){
    delete this->_lightcontroller;
}

//PRIVATE FUNCTIONS 
bool Meta::DefineMaster(){
    if (EEPROM.read(0) == 0)
        return true;
    else
        return false;
}
