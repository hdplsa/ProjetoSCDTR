#include "Error.h"

Error::Error(){

}

void Error::setSerialString(char *str){
    sprintf(this->errorstr,"error: %s",str);
    Serial.println(this->errorstr);
}

Error::~Error(){

}