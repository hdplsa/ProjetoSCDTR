#include "mainController.h"

MainController::MainController(int Narduino, vector<string> ports) : arduino(Narduino,NULL) {
	this->t = 0;
	this->k = 0;
	//Inicialização dos arduinos
	for(int i = 0; i < N; i++){
		arduino[i] = new Arduino(this->N, ports[i]);
	}
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

	int i;

	switch(str.c_str()[0]){
		case 'g': // Get de valores

			if(!(str.c_str()[4] == 'T')){ // Se for um valor de apenas 1 arduino

				i = get_id(str, callback);

				// Se houver erro a ler o id saímos
				if(i == -1) return;

				try{

					double value = -100;
					bool   occupancy;

					// Obtem o valor dependente do segundo parâmetro
					switch(str.c_str()[2]){
						case 'l':
							value = arduino.at(i)->getIlluminance();
							break;	
						case 'd':
							value = arduino.at(i)->getDuty();
							break;
						case 'o':
							occupancy = arduino.at(i)->getOccupancy();
							break;
						case 'L':
							throw "Not implemented";
							break;
						case 'O':
							throw "Not implemented";
							break;
						case 'r':
							value = arduino.at(i)->getRef();
							break;
						case 'p':
							value = arduino.at(i)->getPower();
							break;
						case 'e':
							value = arduino.at(i)->getEnergy();
							break;
						case 'c':
							value = arduino.at(i)->getComfortError();
							break;
						case 'v':
							value = arduino.at(i)->getComfortVariance();
							break;
						default:
							callback("Unknown Command");
							return;
							break;
					}

					// Compõe a string
					if(value == -100){ // Se o value não tiver sido mudado, o valor desejado está na occupancy
						value = (double) occupancy;
					} 

					string send = compose_string(to_string(str.c_str()[2]), to_string(i), value);
					
					// Envia a string
					callback(send);
					
				} catch (std::exception &e){
					// Havendo erro ai em cima, é porque lançado pelo 'at' do arduino, o que indica que não há ess i
					callback("invalid id");
					return;
				}
			} else {	// Neste caso, precisamos de ir a todos os arduinos
				
				try{
					// Soma de todos os valores
					double value = 0;
					for(int i = 0; i < Narduino; i++){
						switch(str.c_str()[2]){
							case 'p':
								value += arduino.at(i)->getPower();
								break;
							case 'e':
								value += arduino.at(i)->getEnergy();
								break;
							case 'c':
								value += arduino.at(i)->getComfortError();
								break;
							case 'v':
								value += arduino.at(i)->getComfortVariance();
								break;
							default:
								callback("Invalid command.");
								return;
								break;
						}
					}

					string send = compose_string(to_string(str.c_str()[2]), "T", value);

					callback(send);
				} catch(std::exception &e){
					callback("Arduino not available");
					return;
				}
			}

		break;

		case 's':
			i = get_id(str, callback, 2);

			try{
				bool value = str.c_str()[str.length()-1] - '0';

				arduino.at(i)->setOccupancy(value);

				cout << "Set occupancy " << value << endl;

				callback("ack");
			} catch (std::exception &e){
				callback("Invalid id");
				return;
			}

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

		break;


	}

}

int MainController::get_id(string str, std::function<void(string)> callback, int start /* = 4*/){
	try{
		int i = std::stoi(str.substr(start,str.length()-1));
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
	for(int i = 0; i < Narduino; i ++){
		delete arduino.at(i);
	}
}