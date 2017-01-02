#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include "SerialCom.h"

class Error{

    public:
        static void setSerialString(char *str);
        static void setSerialString(int d);

    private:
        //Informações sobre o erro
        static int errorcode;
        static char errorstr[100];
};

#endif
