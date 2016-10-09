#include "LightController.h"

LightController::LightController(double Kp,double Ti,double Td){
  this->Kp = Kp;
  this->Ti = Ti;
  this->Td = Td;
}

LightController::~LightController(){
  
}

