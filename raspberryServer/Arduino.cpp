#include "Arduino.h"

Arduino::Arduino(int N_) : N(N_), ref(N_,0), e(N_,0), u(N_,0), y(N_,0), d(N_,0), E(N_,0), Cerror(N_,0), Verror(N_,0) {
	int k;
	this->N = N;
	this->k = 0;
//	//Inicialização de vectores
//	for(k=0; k < this->N;k++){
//		this->ref[k] = 0;
//		this->e[k] = 0;
//		this->u[k] = 0;
//		this->y[k] = 0;
//		this->d[k] = 0;
//		this->E[k] = 0;
//		this->Cerror[k] = 0;
//		this->Verror[k] = 0;
//	}
	this->o = false; // Mudei de 'o1' para 'o' para compilar. Vê se é suposto ser 'o' --- Ass: Hugo
}

/* Implementação dos indices do vector em anel
 * Assim, é necessário determinar os indices 
 * seguintes e anteriores dos extremos do vector */
int Arduino::getkNext(int k){
	if(k >= 0){
		if(k < this->N){
			k = 0;
		} else {
			k = k + 1;
		}
		return k;
	}
	return 0;
}

int Arduino::getkPrevious(int k){
	if(k >= 0){
		if(k == 0){
			k = this->N-1;
		} else {
			k = k - 1;
		}
		return k;
	}
	return 0;
}

/* Calculo dos valores das constantes no modelo 
 * linear  entre tensão e luminosidade l = k u + theta
 * Método de minimos quadrados */
void Arduino::calibrate(){
	
}

/* Calculo do valor do erro no instante k */
void Arduino::calcError(){
	if ((k >= 0) && (k < this->N)){
		this->e[this->k] = this->y[this->k] - this->ref[this->k];
	}
}

/* Calculo da energia gasta nos LEDs */
void Arduino::calcEnergy(){
	if ((k >= 0) && (k < this->N)){
		this->E[this->k] = this->E[this->getkPrevious(this->k)] + this->d[this->getkPrevious(this->k)]*this->T;
	}
}

/* Calculo do erro de comforto com a luminsidade*/
void Arduino::calcComfortError(){
	if ((k >= 0) && (k < this->N)){
		this->Cerror[this->k] = ((this->t-1)/this->t)*getMax(this->e[this->k],0);
	}
}

/* Calculo da variancia de comforto */
void Arduino::calcComfortVariance(){
	double sum1, sum2;
	double sum; // (?)
	if ((k >= 0) && (k < this->N)){
		sum = this->getAbs(this->y[this->k]-2*this->y[this->getkPrevious(this->k)]+this->y[this->getkPrevious(this->getkPrevious(this->k))]);
		this->Verror[this->k] = ((this->t-1)/this->t)*sum/(this->T*this->T);
	}
}

/* Função chamada ciclicamente para receber e guardar dados
 * vindos dos Arduinos */
void Arduino::receiveInformation(){
	//Calcula erro
	this->calcError();
	
	//Avança da o instante seguinte
	this->k = this->getkNext(this->k);
	this->t = this->t + 1;
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