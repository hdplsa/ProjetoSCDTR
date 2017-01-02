#include "Error.h"

Error::Error(){

}

void Error::setSerialString(char *str){
    sprintf(this->errorstr,"error: ");
    strcat(this->errorstr,str);
    Serial.println(this->errorstr);
}

void Error::setSerialString(double d){
    sprintf(this->errorstr,"vec: %g", d);
    Serial.println(this->errorstr);
}

Error::~Error(){

}
