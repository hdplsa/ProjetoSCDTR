#ifndef ARDUINO_H_INCLUDED
#define ARDUINO_H_INCLUDED

#include <iostream>
#include <vector>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "Serial.h"
using namespace std;

#define ARDUINOSIM	 1		// Flag que diz se estamos ou não em modo de simulação (sem arduinos)
#define ARDUINODEBUG 1 		// Flag que exibe as mensagens de debug no terminal

typedef boost::shared_ptr<boost::mutex> shared_mutex;

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
	//vector<long> t;
	//vector<std::chrono::time_point<std::chrono::system_clock>> time;
	vector<double> t;
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
	//External Illuminance
	double theta;
	//Lower bound
	double LowBound;
	
	public:
		Arduino(int N_, string port, shared_mutex mutex_);
		void ArduinoSim();
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
		double getTime();
		double getExternalIlluminance();
		void send(string str);
		void ledON(int pwm = 255);
		void ledON(double V);
		void ledOFF();
		void receiveInformation(char *info);
		void setNewInformationCallback(std::function<void(void)> fcn);
		vector<double> get_minute(vector<double> vec);
		void reset();
		bool saveVectorsCSV(int i);
		~Arduino();
	
	private:
		double getMax(double d1, double d2);
		double getAbs(double d);
		void calcError();
		void calcEnergy();
		void calcComfortError();
		void calcComfortVariance();
		bool savetoCSV(vector<double> vec, string filename);

		// Objeto de serial que vai receber e enviar informações para os arduinos
		Serial *serial;

		// Thread onde corre o serial read e serial write
		boost::thread th;

		// Callback que torna possivel aqueles envios de informação em real time
		std::function<void(void)> newInformationCallback = NULL;

		// Objeto de mutex que nos diz se podemos ou não usar o buffer de saída
		shared_mutex mutex;

		enum sendcodes { Ref = 0 };
};

#endif