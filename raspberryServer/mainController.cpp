#include "mainController.h"

MainController::MainController(){
	this->t = 0;
	this->k = 0;
	//Inicialização dos arduinos
	this->arduino1 = new Arduino(this->N);
	this->arduino2 = new Arduino(this->N);
	//Modelo dos minimos quadrados
	this->k11 = 0;
	this->k12 = 0;
	this->k21 = 0;
	this->k22 = 0;
	this->theta1 = 0;
	this->theta2 = 0;
}

MainController::~MainController(){
	//Desalocação dos arduinos
	delete this->arduino1;
	delete this->arduino2;
}