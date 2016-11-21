#ifndef MAINCONTROLLER_H_INCLUDED
#define MAINCONTROLLER_H_INCLUDED

#include "Arduino.h"

class MainController{
	//Variável que guarda o instante actual
	int t;
	//Variável que guarda a posição actual nos vectores
	int k;
	//Numero de pontos guardados
	int N = 10000;
	//Periodo do ciclo de controlo
	double T;
	//Arduinos
	Arduino *arduino1;
	Arduino *arduino2;
	//Calibração minimos quadrados
	double k11, k12, k21, k22;
	double theta1, theta2;
	

	public:
		MainController();
		int getkNext(int k);
		int getkPrecious(int k);
		void calibrate();
		void calcError();
		void calcEnergy();
		void calcComfortError();
		void receiveInformation();
		~MainController();
		
	private:
		double getMax(double d1, double d2);
		double getAbs();
		
};

#endif