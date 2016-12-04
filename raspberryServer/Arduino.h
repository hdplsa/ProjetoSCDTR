#ifndef ARDUINO_H_INCLUDED
#define ARDUINO_H_INCLUDED

#include <iostream>
#include <vector>
#include <cstdio>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "Serial.h"
using namespace std;

class Arduino{
	//Variável que guarda a posição actual nos vectores
	int k;
	//Numero de pontos guardados
	int N;
	//Periodo do ciclo de controlo
	double T;
	//Valores do sistema fisico
	vector<double> ref;
	//Valores de erro do sistema
	vector<double> e;
	//Valores da variável de controlo
	vector<double> u;
	//Valores de saída do sistema
	vector<double> y;
	//Valores do duty cycle do sistema
	vector<double> d;
	//Valores do tempo em que recebemos cada valor
	vector<boost::posix_time::ptime> t;
	//Valores de consumo de energia
	vector<double> E;
	//Valores de erro de comforto
	vector<double> Cerror;
	//Valores de variância de comforto
	vector<double> Verror;
	//Estado de ocupação
	bool o;
	
	public:
		Arduino(int N_, string port);
		int getkNext(int k);
		int getkPrevious(int k);
		double getEnergy(int k);
		double getComfortError(int k);
		double getComfortVariance(int k);
		double getIlluminance(int k = this->k);
		double getDuty(int k = this->k);
		void send(string str);
		void ledON(int pwm = 255);
		void ledON(float V);
		void ledOFF();
		void receiveInformation(string info);
		~Arduino();
	
	private:
		double getMax(double d1, double d2);
		double getAbs(double d);
		void calcError();
		void calcEnergy();
		void calcComfortError();
		void calcComfortVariance();
		Serial *serial = new Serial();

		enum sendcodes { Ref = 0 };
};

#endif