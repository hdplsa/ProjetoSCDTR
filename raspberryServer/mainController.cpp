#include "mainController.h"

MainController::MainController(){
	this->k = 0;
}

/* Calculo dos valores das constantes no modelo 
 * linear  entre tensão e luminosidade l = k u + theta
 * Método de minimos quadrados */
void MainController::calibrate(){
	
}

/* Calculo do valor do erro no instante k */
void MainController::calcError(int k){
	if (k >= 0){
		this->e[k] = this->y[k] - this->ref[k];
	}
}

/* Calculo da energia gasta nos LEDs */
void MainController::calcEnergy(int k){
	if (k >= 0){
		if (k == 0){
			this->E1[k] = 0;
			this->E2[k] = 0;
		} else {
			this->E1[k] = this->E1[k-1] + this->d1[k-1]*this->T;
			this->E2[k] = this->E2[k-1] + this->d2[k-1]*this->T;
		}
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