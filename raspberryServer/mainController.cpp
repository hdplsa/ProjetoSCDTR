#include "mainController.h"

MainController::MainController(int Narduino, vector<string> ports) : arduino(Narduino,NULL) {
	this->Narduino = Narduino;
	this->t = 0;
	this->k = 0;

	mutex = shared_mutex(new boost::mutex());

	//Inicialização dos arduinos
	for(int i = 0; i < Narduino; i++){
		arduino[i] = new Arduino(this->N, ports[i], mutex);
		arduino[i]->setNewInformationCallback(std::bind(&MainController::printMetrics, this, i));
		//inicialização da callback periodica
		realtimecallbacks[std::make_pair(i,'l')] = std::list<std::pair<std::function<void(string)>, void*>>();
		realtimecallbacks[std::make_pair(i,'d')] = std::list<std::pair<std::function<void(string)>, void*>>();
	}
}

void MainController::printMetrics(int i){

	vector<char> comandos = {'l', 'd'};
	std::function<void(string)> send = NULL;
	double value = -100;
	string str;

	// Itera pelos comandos
	for (std::vector<char>::iterator it = comandos.begin(); it != comandos.end(); ++it){

		// Obtém a lista com os callbacks e os ponteiros para a session
		std::list< std::pair< std::function<void(string)>, void* > > lista = realtimecallbacks[std::make_pair(i,*it)];

		// Itera na lista de callbacks
		for(std::list< std::pair< std::function<void(string)>, void*>>::iterator it2 = lista.begin(); it2 != lista.end(); ++it2){
			cout << "Lista com: " << lista.size() << endl;

			// Obtem o callback da lista
			std::function<void(string)> send = std::get<0>(*it2);
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
				
				str = "c ";
				str += *it; str += ' '; str += to_string(i); str += ' ';
				str += to_string(value); str += ' '; 
				str += to_string(arduino.at(i)->getTime()); str += '\n';

				send(str);
			}
		}
	}
}

// Esta função processa o input que o cliente manda para o server e responde
// com a informação que o cliente pediu. A resposta é feita para o callback
// que esta função recebe.
void MainController::get_clientRequest(string str, std::function<void(string)> callback, void* object_ptr){

	int i;

	// Verifica o primeiro caracter
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
					// Compõe a string final a ser enviada
					string send = compose_string(string(1,str.c_str()[2]), "T", value);

					// Envia a string
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

				callback("ack\n");
			} catch (std::exception &e){
				callback("Invalid id\n");
				return;
			}

		break;

		case 'r':

		cout << "Faz reset do sistema" << endl;

		try{
			for(int i = 0; i < Narduino; i++){
				arduino.at(i)->reset();
			}
			for(int i = 0; i < Narduino; i++){
				arduino.at(i)->InitArduino();
			}


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
				value_str += str[2]; value_str += " ";
				value_str += str[4]; value_str += " ";

				for(unsigned int i = 0; i < value.size() - 1; i++){
					value_str += to_string(value.at(i));
					value_str += ",";
				}
				value_str += to_string(value.at(i+1));

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
					case 'd':
						compare_funcs(&realtimecallbacks[std::make_pair(i,str.c_str()[2])], object_ptr);
						realtimecallbacks[std::make_pair(i,str.c_str()[2])].push_front(std::make_pair(callback,object_ptr));
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
					case 'd':
						compare_funcs(&realtimecallbacks[std::make_pair(i,str.c_str()[2])], object_ptr);
						break;
					default:
						callback("Invalid command\n");
						return;
			}

		break;

		case 'm':
				cout << "Print CSV" << endl;
				for(i=0; i < this->Narduino; i++){
					this->arduino[i]->saveVectorsCSV(i);
				}
		break;

		default:

			callback("Invalid command\n");

		break;

	}
}

void MainController::compare_funcs(std::list< std::pair< std::function<void(string)>, void* > > *lista, void* object_ptr){
	cout << lista->size() << endl;

	std::list<std::pair<std::function<void(string)>, void*>>::iterator it = lista->begin();

	while(it != lista->end()){

		if(lista->empty()) break;

		void* object_list = std::get<1>(*it);
		if(object_ptr == object_list){
			auto it_aux = ++it;
			--it;
			lista->erase(it);
			cout << "Apaguei 1" << endl;
			it = it_aux;
		} else ++it;

	}

	cout << "Lista com: " << lista->size() << endl;
}

void MainController::delete_realtime(void* object_ptr){

	// Temos que iterar pelos comandos, pelo numero de arduinos e pelo vetor de callbacks
	// e identificar se há callbacks desse client ou não	

	vector<char> comandos = {'l', 'd'};

	for (int i = 0; i < Narduino; i++){ //Itera no numero de arduinos
		for (std::vector<char>::iterator it = comandos.begin(); it != comandos.end(); ++it){ //Itera nos comandos

			compare_funcs(&realtimecallbacks[std::make_pair(i,*it)], object_ptr);
			
		}
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

bool MainController::isCalibrated(){
	int i;
	double ret;
	for(i=0,ret=0; i < this->Narduino; i++){
		ret = ret*this->arduino.at(i)->isCalibration();
	}
	return ret;
}

MainController::~MainController(){
	//Desalocação dos arduinos
	for(int i = 0; i < Narduino; i ++){
		delete arduino.at(i);
	}
}