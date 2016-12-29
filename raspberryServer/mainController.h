#ifndef MAINCONTROLLER_H_INCLUDED
#define MAINCONTROLLER_H_INCLUDED

#include <stdio.h>
#include <iostream>
#include "Arduino.h"

#if defined __unix__
	//https://www.mathworks.com/help/matlab/mex-library.html
	#define MATLAB true
#endif

using namespace std;

typedef boost::shared_ptr<boost::mutex> shared_mutex;

class MainController{
	//Variável que guarda o instante actual
	int t;
	//Variável de instante actual no vector
	int k;
	//Numero de pontos guardados
	int N = 1000;
	// Numero de arduinos
	int Narduino;
	//Arduinos
	vector<Arduino*> arduino;
	//Flag de calibração
	bool systemCalibration;
	
	public:
		MainController(int Narduino, vector<string> ports);
		void get_clientRequest(string str, std::function<void(string)> callback);
		void delete_realtime(std::function<void(string)> fcn);
		void printMetrics(int Arduino);
		bool isCalibrated();
		~MainController();
		
	private:
		int get_id(string str, std::function<void(string)> callback, int start = 4);
		string compose_string(string param1, string param2, double val);
		std::map<std::pair<int, char>, std::list<std::function<void(string)>>> realtimecallbacks;
		void compare_funcs(std::list<std::function<void(string)>> *lista, std::function<void(string)> fcn);
		
		//Mutex que impede os arduinos de imprimir ao mesmo tempo
		shared_mutex mutex;
		
};

#endif