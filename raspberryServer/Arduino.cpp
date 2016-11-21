#include "Arduino.h"

Arduino::Arduino(int N){
	int k;
	this->N = N;
	//Inicialização de vectores
	for(k=0; k < this->N;k++){
		this->ref[k] = 0;
		this->e[k] = 0;
		this->u[k] = 0;
		this->y[k] = 0;
		this->d[k] = 0;
		this->E[k] = 0;
		this->Cerror[k] = 0;
		this->Verror[k] = 0;
	}
	this->o1 = false;
}

Arduino::~Arduino(){
	
}

/* Retorna o valor máximo entre d1 e d2
 * Caso d1 = d2, retorna 0 */
double Arduino::getMax(double d1, double d2){
	if (d1 > d2){
		return d1;
	}
	if (d2 > d1){
		return d2;
	}
	return 0;
}

double Arduino::getAbs(double d){
	if(d >= 0){
		return d;
	} else {
		return -1*d;
	}
}