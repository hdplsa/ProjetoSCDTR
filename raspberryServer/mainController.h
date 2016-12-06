#ifndef MAINCONTROLLER_H_INCLUDED
#define MAINCONTROLLER_H_INCLUDED

#include <stdio.h>
#include <iostream>
#include "Arduino.h"

using namespace std;

class MainController{
	//Variável que guarda o instante actual
	int t;
	//Variável de instante actual no vector
	int k;
	//Numero de pontos guardados
	int N = 10000;
	// Numero de arduinos
	int Narduino;
	//Arduinos
	vector<Arduino*> arduino;
	//Calibração minimos quadrados
	double k11, k12, k21, k22;
	double theta1, theta2;
	
	public:
		MainController(int Narduino, vector<string> ports);
		void get_clientRequest(string str, std::function<void(string)> callback);
		void printMetrics();
		~MainController();
		
	private:
		int get_id(string str, std::function<void(string)> callback, int start = 4);
		string compose_string(string param1, string param2, double val);
		
};

#endif