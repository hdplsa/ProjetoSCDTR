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

MainController::~MainController(){
	
}