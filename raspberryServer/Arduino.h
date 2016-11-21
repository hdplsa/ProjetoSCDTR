#ifndef ARDUINO_H_INCLUDED
#define ARDUINO_H_INCLUDED

#include <iostream>
#include <vector>
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
	//Valores de consumo de energia
	vector<double> E;
	//Valores de erro de comforto
	vector<double> Cerror;
	//Valores de variância de comforto
	vector<double> Verror;
	//Estado de ocupação
	bool o;

	// (?) Parâmetro adicionado para compilar NÃO ESTÁ NO CONSTRUTOR
	int t;
	
	public:
		Arduino(int N_);
		int getkNext(int k);
		int getkPrevious(int k);
		void calibrate();
		void calcError();
		void calcEnergy();
		void calcComfortError();
		void calcComfortVariance();
		void receiveInformation();
		~Arduino();
	
	private:
		double getMax(double d1, double d2);
		double getAbs(double d);
};

#endif