#include "mainController.h"

MainController::MainController(){
	int k;
	this->t = 0;
	this->k = 0;
	//Inicialização de vectores
	for(k=0; k < this->N;k++){
		this->ref1[k] = 0;
		this->ref2[k] = 0;
		this->e1[k] = 0;
		this->e2[k] = 0;
		this->u1[k] = 0;
		this->u2[k] = 0;
		this->y1[k] = 0;
		this->y2[k] = 0;
		this->d1[k] = 0;
		this->d2[k] = 0;
		this->E1[k] = 0;
		this->E2[k] = 0;
		this->Cerror1[k] = 0;
		this->Cerror2[k] = 0;	
	}
	this->o1 = false;
	this->o2 = false;
	this->k11 = 0;
	this-> k12 = 0;
	this->k21 = 0;
	this->k22 = 0;
	this->theta1 = 0;
	this->theta2 = 0;
}

/* Implementação dos indices do vector em anel
 * Assim, é necessário determinar os indices 
 * seguintes e anteriores dos extremos do vector */

int MainController::getkNext(int k){
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

int MainController::getkPrevious(int k){
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
void MainController::calibrate(){
	
}

/* Calculo do valor do erro no instante k */
void MainController::calcError(){
	if ((k >= 0) && (k < this->N)){
		this->e[this->k] = this->y[this->k] - this->ref[this->k];
	}
}

/* Calculo da energia gasta nos LEDs */
void MainController::calcEnergy(){
	if ((k >= 0) && (k < this->N)){
		this->E1[this->k] = this->E1[this->getkPrevious(this->k)] + this->d1[this->getkPrevious(this->k)]*this->T;
		this->E2[this->k] = this->E2[this->getkPrevious(this->k)] + this->d2[this->getkPrevious(this->k)]*this->T;
	}
}

/* Calculo do erro de comforto com a luminsidade*/
void MainController::calcComfortError(int k){
	if (k >= 0){
		if (k == 0){
			this->Cerror1[k] = 0;
			this->Cerror2[k] = 0;
		} else {
			this->Cerror1[k] = ((k-1)/k)*getMax(this->ref1[k]-this->y1[k],0);
			this->Cerror2[k] = ((k-1)/k)*getMax(this->ref2[k]-this->y2[k],0);
		}
	}
}

/* Função chamada ciclicamente para receber e guardar dados
 * vindos dos Arduinos */
void MainController::receiveInformation(){
	
	//Avança da o instante seguinte
	this->k = this->getkNext(this->k);
	this->t = this->t + 1;
}

MainController::~MainController(){
	
}

/* Retorna o valor máximo entre d1 e d2
 * Caso d1 = d2, retorna 0 */
double MainController::getMax(double d1, double d2){
	if (d1 > d2){
		return d1;
	}
	if (d2 > d1){
		return d2;
	}
	return 0;
}