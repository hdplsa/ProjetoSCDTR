#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include "SerialCom.h"

class Error{
    //Informações sobre o erro
    int errorcode;
    char errorstr[100];

    public:
        Error();
        void setSerialString(char *str);
        void setSerialString(int d);
        ~Error();
};

#endif
