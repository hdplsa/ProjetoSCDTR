#ifndef ARDUINO_H_INCLUDED
#define ARDUINO_H_INCLUDED

#include <iostream>
#include <vector>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "Serial.h"
using namespace std;
//using namespace boost::posix_time;

class Arduino{
	//Variável que guarda a posição actual nos vectores
	int K;
	//Numero de pontos guardados
	int N;
	//Numero de ciclos desde o inicio
	long cycle = 1;
	//Periodo do ciclo de controlo
	double T = 0.02;
	//Valores de t do sistema fisico
	vector<long> t;
	//Valores de referencia do sistema fisico
	vector<double> ref;
	//Valores de erro do sistema
	vector<double> e;
	//Valores da variável de controlo
	vector<double> u;
	//Valores de saída do sistema
	vector<double> y;
	//Valores do duty cycle do sistema
	vector<double> d;
	//Valores de consumo de energia
	vector<double> E;
	//Valores de erro de comforto
	vector<double> Cerror;
	//Valores de variância de comforto
	vector<double> Verror;
	//Estado de ocupação
	bool o;
	//Lower bound
	double LowBound;
	
	public:
		Arduino(int N_, string port);
		int getkNext(int k);
		int getkPrevious(int k);
		double getReference();
		double getEnergy();
		double getEnergy(int k);
		double getComfortError();
		double getComfortError(int k);
		double getComfortVariance();
		double getComfortVariance(int k);
		double getIlluminance();
		double getIlluminance(int k);
		vector<double> getLastMinuteIlluminance();
		double getDuty();
		double getDuty(int k);
		vector<double> getLastMinuteDuty();
		bool getOccupancy();
		double getLowerBoundIlluminance();
		void setOccupancy(bool value);
		double getRef();
		double getPower(); // Nâo implementada
		long getTime();
		void send(string str);
		void ledON(int pwm = 255);
		void ledON(double V);
		void ledOFF();
		void receiveInformation(string info);
		void setNewInformationCallback(std::function<void(void)> fcn);
		vector<double> get_minute(vector<double> vec);
		void reset();
		~Arduino();
	
	private:
		double getMax(double d1, double d2);
		double getAbs(double d);
		void calcError();
		void calcEnergy();
		void calcComfortError();
		void calcComfortVariance();
		Serial *serial;

		boost::thread th;

		std::function<void(void)> newInformationCallback = NULL;

		enum sendcodes { Ref = 0 };
};

#endif