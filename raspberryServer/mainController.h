#ifndef MAINCONTROLLER_H_INCLUDED
#define MAINCONTROLLER_H_INCLUDED

class MainController{
	//Numero de pontos guardados
	int N = 200;
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
	//Estado de ocupação
	bool o1;
	bool o2;
	

public:
		MainController();
		~MainController();
};

#endif