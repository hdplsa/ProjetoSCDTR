#ifndef ARDUINO_H_INCLUDED
#define ARDUINO_H_INCLUDED

class Arduino{
	//Variável que guarda a posição actual nos vectores
	int k;
	//Numero de pontos guardados
	int N;
	//Periodo do ciclo de controlo
	double T;
	//Valores do sistema fisico
	double ref[N];
	//Valores de erro do sistema
	double e[N];
	//Valores da variável de controlo
	double u[N];
	//Valores de saída do sistema
	double y[N];
	//Valores do duty cycle do sistema
	double d[N];
	//Valores de consumo de energia
	double E[N];
	//Valores de erro de comforto
	double Cerror[N];
	//Valores de variância de comforto
	double Verror[N];
	//Estado de ocupação
	bool o;
	
	public:
		Arduino(int N);
		int getkNext(int k);
		int getkPrecious(int k);
		void calibrate();
		void calcError();
		void calcEnergy();
		void calcComfortError();
		void receiveInformation();
		~Arduino();
	
	private:
		double getMax(double d1, double d2);
		double getAbs();
};

#endif