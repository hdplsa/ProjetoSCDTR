#ifndef MAINCONTROLLER_H_INCLUDED
#define MAINCONTROLLER_H_INCLUDED

#include "Arduino.h"

class MainController{
	//Variável que guarda o instante actual
	int t;
	//Numero de pontos guardados
	int N = 10000;
	//Arduinos
	Arduino *arduino1;
	Arduino *arduino2;
	//Calibração minimos quadrados
	double k11, k12, k21, k22;
	double theta1, theta2;
	

	public:
		MainController();
		~MainController();
		
	private:
		
};

#endif