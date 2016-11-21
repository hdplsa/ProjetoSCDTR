#include "mainController.h"

MainController::MainController(){
	this->t = 0;
	this->k = 0;
	//Modelo dos minimos quadrados
	this->k11 = 0;
	this->k12 = 0;
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
void MainController::calcComfortError(){
	if ((k >= 0) && (k < this->N)){
		this->Cerror1[this->k] = ((this->t-1)/this->t)*getMax(this->e1[this->k],0);
		this->Cerror2[this->k] = ((this->t-1)/this->t)*getMax(this->e2[this->k],0);
	}
}

/* Calculo da variancia de comforto */
void MainController::calcComfortVariance(){
	double sum1, sum2;
	if ((k >= 0) && (k < this->N)){
		sum1 = this->getAbs(this->y1[this->k]-2*this->y1[this->getkPrevious(this->k)]+this->y1[this->getkPrevious(this->getkPrevious(this->k))]);
		sum2 = this->getAbs(this->y2[this->k]-2*this->y2[this->getkPrevious(this->k)]+this->y2[this->getkPrevious(this->getkPrevious(this->k))]);
		this->Verror1[this->k] = ((this->t-1)/this->t)*sum1/(this->T*this->T);
		this->Verror2[this->k] = ((this->t-1)/this->t)*sum2/(this->T*this->T);
	}
}

/* Função chamada ciclicamente para receber e guardar dados
 * vindos dos Arduinos */
void MainController::receiveInformation(){
	//Calcula erro
	this->calcError();
	
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

double MainController::getAbs(double d){
	if(d >= 0){
		return d;
	} else {
		return -1*d;
	}
}