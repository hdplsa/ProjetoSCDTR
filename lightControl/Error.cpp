#include "Error.h"

Error::Error(){

}

void Error::setSerialString(char *str){
    sprintf(this->errorstr,"error: ");
    strcat(this->errorstr,str);
    Serial.println(this->errorstr);
}

void Error::setSerialString(int d){
    sprintf(this->errorstr,"vec: %d", d);
    Serial.println(this->errorstr);
}

Error::~Error(){

}
