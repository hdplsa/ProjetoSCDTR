#ifndef MAINCONTROLLER_H_INCLUDED
#define MAINCONTROLLER_H_INCLUDED

#include <stdio.h>

class MainController{
	//Variável que guarda o instante actual
	int t;
	//Variável de instante actual no vector
	int k;
	//Numero de pontos guardados
	int N = 10000;
	//Arduinos
	Arduino *arduino1;
	Arduino *arduino2;
	// Portas dos arduinos
	const string port1 = "/dev/ttyUSB0";
	const string port2 = "/dev/ttyUSB1";
	//Calibração minimos quadrados
	double k11, k12, k21, k22;
	double theta1, theta2;
	

	public:
		MainController();
		void get_clientRequest(string str, std::function<void(string)> callback);
		void printMetrics();
		~MainController();
		
	private:
		void get_id(string str, std::function<void(string)> callback);
		string compose_string(string param1, string param2, double val);
		
};

#endif