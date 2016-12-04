#include "mainController.h"

MainController::MainController(){
	this->t = 0;
	this->k = 0;
	//Inicialização dos arduinos
	this->arduino1 = new Arduino(this->N, port1);
	this->arduino2 = new Arduino(this->N, port2);
	//Modelo dos minimos quadrados
	this->k11 = 0;
	this->k12 = 0;
	this->k21 = 0;
	this->k22 = 0;
	this->theta1 = 0;
	this->theta2 = 0;
}

void MainController::printMetrics(){
	
}

void MainController::get_clientRequest(string str, std::function<void(string)> callback){

	switch(str[0]){
		case 'g':
			switch(str[1]){
				case 'l':

					int i = get_id(str, callback);

					// Se houver erro a ler o id saímos
					if(i == -1) return;

					try{
						double val = arduino.at(i).getIlluminance();

						string send = compose_string(string("l"), string(i), val);
					} catch (std::exception &e){
						callback("invalid id");
					}

				break;

				case 'd':
					int i = get_id(str, callback);

					// Se houver erro a ler o id saímos
					if(i == -1) return;

					try{
						double val = arduino.at(i).getIlluminance();

						string send = compose_string(string("d"), string(i), val);
					} catch (std::exception &e){
						callback("invalid id");
					}
				break;

				case 'o':

				break;

				case 'L':

				break;

				case 'O':

				break;

				case 'r':

				break;
				
				case 'p':

				break;

				case 'e':

				break;

				case 'c':

				break;

				case 'v':

				break;

			}

		break;

		case 's':

		break;

		case 'r':

		break;

		case 'b':

		break;

		case 'c':
			/* Forma de obter timestamp em c/ milisegundos em http://stackoverflow.com/questions/16077299/how-to-print-current-time-with-milliseconds-using-c-c11
			*/
		break;

		case 'd':

		break;

		default:

			// Enviar invalid command

		break


	}

}

void MainController::get_id(string str, std::function<void(string)> callback){
	try{
		int i = std::stoi(str.substr(3,str.length()-1));
		return i;

	} catch(std::exception e){
		return -1;
	}
	
}

string MainController::compose_string(string param1, string param2, double val){

	string str = param1;

	str += ' ';
	str += param2;
	str += ' ';
	str += val;

	return str;

}

MainController::~MainController(){
	//Desalocação dos arduinos
	delete this->arduino1;
	delete this->arduino2;
}