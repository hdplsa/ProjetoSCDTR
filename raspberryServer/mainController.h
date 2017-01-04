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
		void get_clientRequest(string str, std::function<void(string)> callback, void* object_ptr);
		void delete_realtime(void* object_ptr);
		void printMetrics(int Arduino);
		bool isCalibrated();
		~MainController();
		
	private:
		int get_id(string str, std::function<void(string)> callback, int start = 4);
		string compose_string(string param1, string param2, double val);

		std::map<std::pair<int, char>, std::list< std::pair< std::function<void(string)>, void* > >> realtimecallbacks;
		void compare_funcs(std::list< std::pair< std::function<void(string)>, void* > > *lista, void* object_ptr);
		
		//Mutex que impede os arduinos de imprimir ao mesmo tempo
		shared_mutex mutex;

		vector<double> sumVectors(vector<double> d1, vector<double> d2);
		bool savetoCSV(vector<double> vec, string filename);
		
};

#endif