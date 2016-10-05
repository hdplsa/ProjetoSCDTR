#include "LumItensity.h"

LumItensity::LumItensity(){
  this->Rs = 0;
  this->lumIt = 0;
}

void LumItensity::setSensorResistance(double Rs){
  this->Rs = Rs;
}

double LumItensity::getLuminousItensity(){
  return this->lumIt;
}

LumItensity::~LumItensity(){
  
}

