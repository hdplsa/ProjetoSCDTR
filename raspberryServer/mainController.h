#ifndef MAINCONTROLLER_H_INCLUDED
#define MAINCONTROLLER_H_INCLUDED

class MainController{
	//Variável que guarda o instante actual
	int k;
	//Numero de pontos guardados
	int N = 10000;
	//Periodo do ciclo de controlo
	double T;
	//Periodo de amostragem
	double TPWM;
	//Valores do sistema fisico
	double ref1[N];
	double ref2[N];
	//Valores de erro do sistema
	double e1[N];
	double e2[N];
	//Valores da variável de controlo
	double u1[N];
	double u2[N];
	//Valores de saída do sistema
	double y1[N];
	double y2[N];
	//Valores do duty cycle do sistema
	double d1[N];
	double d2[N];
	//Valores de consumo de energia
	double E1[N];
	double E2[N];
	//Valores de erro de comforto
	double Cerror1[N];
	double Cerror2[N];
	//Estado de ocupação
	bool o1;
	bool o2;
	//Calibração minimos quadrados
	double k11, k12, k21, k22;
	double theta1, theta2;
	

	public:
		MainController();
		void calibrate();
		void calcError();
		~MainController();
		
	private:
		double getMax(double d1, double d2);
		
};

#endif