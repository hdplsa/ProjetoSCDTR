#include "mainController.h"

MainController::MainController(int Narduino, vector<string> ports) : arduino(Narduino,NULL) {
	this->Narduino = Narduino;
	/*this->t = 0;
	this->k = 0;
	//Inicialização dos arduinos
	for(int i = 0; i < Narduino; i++){
		arduino[i] = new Arduino(this->N, ports[i]);
		//inicialização da callback periodica
		realtimecallbacks[std::make_pair(i,'l')] = NULL;
		realtimecallbacks[std::make_pair(i,'d')] = NULL;
		arduino[i]->setNewInformationCallback(std::bind(&MainController::printMetrics, this, i));
	}*/
	this->Init();
}

void MainController::Init(){
	this->t = 0;
	this->k = 0;
	//Inicialização dos arduinos
	for(int i = 0; i < Narduino; i++){
		arduino[i] = new Arduino(this->N, ports[i]);
		//inicialização da callback periodica
		realtimecallbacks[std::make_pair(i,'l')] = NULL;
		realtimecallbacks[std::make_pair(i,'d')] = NULL;
		arduino[i]->setNewInformationCallback(std::bind(&MainController::printMetrics, this, i));
	}
}

void MainController::printMetrics(int i){

	vector<char> comandos = {'l', 'd'};
	std::function<void(string)> send = NULL;
	double value = -100;
	string str = "c ";

	// Itera pelos comandos
	for (std::vector<char>::iterator it = comandos.begin(); it != comandos.end(); ++it){
		// Obtém o callback
		send = realtimecallbacks[std::make_pair(i,*it)];
		if(send != NULL){
			//Obtém o valor que temos que mandar
			switch(*it){
				case 'l':
					value = arduino.at(i)->getIlluminance();
					break;	
				case 'd':
					value = arduino.at(i)->getDuty();
					break;
			}

			str += *it; str += ' '; str += to_string(i); str += ' ';
			str += to_string(value); str += ' '; 
			str += arduino.at(i)->getTime(); str += '\n';

			send(str);
		}
	}
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
							value = arduino.at(i)->getLowerBoundIlluminance();
							break;
						case 'O':
							value = arduino.at(i)->getExternalIlluminance();
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
							callback("Unknown Command\n");
							return;
							break;
					}

					// Compõe a string
					if(value == -100){ // Se o value não tiver sido mudado, o valor desejado está na occupancy
						value = (double) occupancy;
					} 

					string param1(1,str.c_str()[2]);

					string send = compose_string(param1, to_string(i), value);
					
					// Envia a string
					callback(send);
					
				} catch (std::exception &e){
					// Havendo erro ai em cima, é porque lançado pelo 'at' do arduino, o que indica que não há ess i
					callback("invalid id\n");
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
								callback("Invalid command.\n");
								return;
								break;
						}
					}

					string param1(1,str.c_str()[2]);

					string send = compose_string(param1, "T", value);

					callback(send);
				} catch(std::exception &e){
					callback("Arduino not available\n");
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
				callback("Invalid id\n");
				return;
			}

		break;

		case 'r':

		cout << "Faz reset do sistema" << endl;

		try{
			for(int i = 0; i < Narduino; i++){
				delete arduino.at(i);
			}

			this->Init();

			callback("ack\n");
		} catch (std::exception &e) {
			cout << "Erro " << e.what() << endl;
			callback("Unknown error occured\n");
			return;
		}

		break;

		// Daqui para a frente são os novos comandos

		case 'b':

			i = get_id(str, callback, 4);

			try {

				vector<double> value;

				switch(str.c_str()[2]){
					case 'l':
						value = arduino.at(i)->getLastMinuteIlluminance();
						break;
					case 'd':
						value = arduino.at(i)->getLastMinuteDuty();
						break;
					default:
						callback("Invalid command\n");
						return;
				}

				string value_str = "b ";
				value_str += str[2];

				for(vector<double>::iterator it = value.begin(); it != value.end(); it++){
					value_str += to_string(*it);
					value_str += ",";
				}

				value_str += "\n";

				callback(value_str);

			} catch(std::exception &e){
				callback("Invalid id\n");
				return;
			}

		break;

		case 'c':
			/* Forma de obter timestamp em c/ milisegundos em http://stackoverflow.com/questions/16077299/how-to-print-current-time-with-milliseconds-using-c-c11
			*/

			i = get_id(str, callback, 4);

			switch(str.c_str()[2]){
					case 'l':
						realtimecallbacks[std::make_pair(i,str.c_str()[2])] = callback;
						break;
					case 'd':
						realtimecallbacks[std::make_pair(i,str.c_str()[2])] = callback;
						break;
					default:
						callback("Invalid command\n");
						return;
			}

		break;

		case 'd':

			i = get_id(str, callback, 4);

			switch(str.c_str()[2]){
					case 'l':
						realtimecallbacks[std::make_pair(i,str.c_str()[2])] = NULL;
						break;
					case 'd':
						realtimecallbacks[std::make_pair(i,str.c_str()[2])] = NULL;
						break;
					default:
						callback("Invalid command\n");
						return;
			}

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
	str += to_string(val);
	str += '\n';

	return str;

}

MainController::~MainController(){
	//Desalocação dos arduinos
	for(int i = 0; i < Narduino; i ++){
		delete arduino.at(i);
	}
}