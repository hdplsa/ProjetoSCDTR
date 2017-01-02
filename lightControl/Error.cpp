#include "Error.h"

int Error::errorcode;
char Error::errorstr[100] = {'\0'};

void Error::setSerialString(char *str){
    sprintf(errorstr,"error: ");
    strcat(errorstr,str);
    Serial.println(errorstr);
}

void Error::setSerialString(int d){
    sprintf(errorstr,"vec: %d", d);
    Serial.println(errorstr);
}

