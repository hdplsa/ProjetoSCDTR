#include "Arduino.h"

Arduino::Arduino(int N_, string port, shared_mutex mutex_) : N(N_), t(N_,0), ref(N_,0), e(N_,0), u(N_,0), y(N_,0), d(N_,0), E(N_,0), Cerror(N_,0), Verror(N_,0), mutex(mutex_) {

	//Valores iniciais
	this->K = 0;
	this->o = true;
	this->LowBound = 10; // <------------------------- Verficar

	if(ARDUINOSIM){

		th = boost::thread(boost::bind(&Arduino::ArduinoSim,this));

	} else {

		// Abre a porta serial
		serial = new Serial();
		serial->Begin(115200, port.c_str());

		// Atribui os callbacks
		serial->set_Readcallback(std::bind(&Arduino::receiveInformation, this, std::placeholders::_1));

		// Começa a leitura do serial
		th = boost::thread(boost::bind(&Serial::read_ln,serial));

	}
	
}

void Arduino::ArduinoSim(){

	// Set de valores iniciais

	while(1){ //Continua a enviar valores até se fechar o programa

		// Guarda os dados no objeto
		if(o){
			this->ref[K] = 50.0;
		} else {
			this->ref[K] = 10.0;
		}

		this->d[K] = 100;
		this->y[K] = 100;
		this->e[K] = 10;	
		this->theta = 20;
		//Calculo o vector de tempo
		auto time_epoch = boost::chrono::steady_clock::now().time_since_epoch();
		double time_millis = boost::chrono::duration_cast<boost::chrono::milliseconds>(time_epoch).count();
		double time_sec = boost::chrono::duration_cast<boost::chrono::seconds>(time_epoch).count();
		this->t[K] = time_millis;
		//Cálculo de novas métricas
		this->calcEnergy();
		this->calcComfortError();
		this->calcComfortVariance();
		//Avança da o instante seguinte
		this->K = this->getkNext(this->K);
		this->cycle ++;

		// Envia os dados para o cout
		if (ARDUINODEBUG){
			mutex->lock();
			cout << "Ciclo = " << to_string(cycle) << endl;
			cout << "data " << this->ref[K] << " " << this->d[K] << " " << this->e[K] << " " << this->theta << endl;
			cout << "Energy = " << this->E[this->getkPrevious(K)] << endl;
			cout << "Cerror = " << this->Cerror[this->getkPrevious(K)] << endl;
			cout << "Verror = " << this->Verror[this->getkPrevious(K)] << endl;
			cout << "T " << to_string(this->t[getkPrevious(K)]) << endl;
			cout << "BoostT " << to_string(time_sec) << " " << to_string(time_millis) << endl;
			cout << endl;
			mutex->unlock();
		}
		//Chama função callback
		if(newInformationCallback != NULL) newInformationCallback();

		// Faz esta threda dormir X milisegundos, assim simula o comportamento do arduino
		boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

	}

}

/* Implementação dos indices do vector em anel
 * Assim, é necessário determinar os indices 
 * seguintes e anteriores dos extremos do vector */
int Arduino::getkNext(int k){
	if(k >= 0){
		if(k < this->N){
			k = k + 1;
		} else {
			k = 0;
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

double Arduino::getReference(){
	return this->ref[getkPrevious(this->K)];
}

double Arduino::getEnergy(){
	return this->E[getkPrevious(this->K)];
}

double Arduino::getEnergy(int k){
	return this->E[k];
}

double Arduino::getComfortError(){
	return this->Cerror[getkPrevious(this->K)];
}

double Arduino::getComfortError(int k){
	return this->Cerror[k];
}

double Arduino::getComfortVariance(){
	return this->Verror[getkPrevious(this->K)];
}

double Arduino::getComfortVariance(int k){
	return this->Verror[k];
}

double Arduino::getIlluminance(){
	return this->y[getkPrevious(this->K)];
}

double Arduino::getIlluminance(int k){
	return this->y[k];
}

vector<double> Arduino::getLastMinuteIlluminance(){
	vector<double> sum = get_minute(y);

	return sum;
}

double Arduino::getDuty(){
	return this->d[getkPrevious(this->K)];
}

double Arduino::getDuty(int k){
	return this->d[k];
}

vector<double> Arduino::getLastMinuteDuty(){

	vector<double> sum = get_minute(d);

	return sum;
}

bool Arduino::getOccupancy(){
	return this->o;
}

double Arduino::getLowerBoundIlluminance(){
	return this->LowBound;
}

void Arduino::setOccupancy(bool value){
	string send = "o ";
	send += to_string(value);
	this->o = value;

	if(!ARDUINOSIM){
		this->serial->Write(send);
	}

}

double Arduino::getRef(){
	return this->ref[getkPrevious(K)];
}

void Arduino::send(string str){

	if(!ARDUINOSIM){
		serial->Write(str);
	}
	
}

double Arduino::getPower(){
	
	int prevK = getkPrevious(this->K);
	int prevK2 = getkPrevious(prevK);
	double deltaE;
	long deltat;

	deltaE = E[prevK] - E[prevK2];
	
	deltat = t[prevK] - t[prevK2];

	return deltaE/((double)((double)deltat/1000.0));

}

double Arduino::getTime(){
	return t[getkPrevious(this->K)];
}

double Arduino::getExternalIlluminance(){
	return this->theta;
}

void Arduino::ledON(int pwm /* = 255 */){

	string str;
	sendcodes s = Ref;

	str += std::to_string(s);
	str += " ";
	str += std::to_string(pwm);
	str += '\n';

	cout << "ledOn " << str << endl;

	send(str);

}

void Arduino::ledON(double V){

	int valor = (int)(V*255.0/5.0);

	ledON(valor);

}

void Arduino::ledOFF(){

	ledON(0);

}

/* Função chamada ciclicamente para receber e guardar dados
 * vindos dos Arduinos */
void Arduino::receiveInformation(char *info){
	int ref;
	int dc;
	float y;
	float e;
	float theta;
	int dataN;

	// Extrai os dados da string
	if(dataN = sscanf(info, "data %d %d %f %f %f", &ref, &dc, &y, &e, &theta) == 5){
		// Guarda os dados no objeto
		this->ref[K] = ref;
		this->d[K] = dc;
		this->y[K] = y;
		this->e[K] = e;	
		this->theta = theta;
		// Obtem o tempo do boost::chrono
		auto time_epoch = boost::chrono::steady_clock::now().time_since_epoch();
		double time_millis = boost::chrono::duration_cast<boost::chrono::milliseconds>(time_epoch).count();
		//Calculo o vector de tempo
		this->t[K] = this->t[this->getkPrevious(K)] + this->T;
		//Cálculo de novas métricas
		this->calcEnergy();
		this->calcComfortError();
		this->calcComfortVariance();
		//Avança da o instante seguinte
		this->K = this->getkNext(this->K);
		this->cycle = this->cycle + 1;

		if (ARDUINODEBUG){
			mutex->lock();
			cout << "Ciclo = " << to_string(cycle) << endl;
			cout << "data " << this->ref[K] << " " << this->d[K] << " " << this->e[K] << " " << this->theta << endl;
			cout << "Energy = " << this->E[this->getkPrevious(K)] << endl;
			cout << "Cerror = " << this->Cerror[this->getkPrevious(K)] << endl;
			cout << "Verror = " << this->Verror[this->getkPrevious(K)] << endl;
			cout << endl;
			mutex->unlock();
		}
		//Chama função callback
		if(newInformationCallback != NULL) newInformationCallback();
	} else {
		//Recepção de mensagens de erro
		char errorString[100];
		if(sscanf(info, "error: %s", errorString) == 1){
			//Processamento da string de erro
			cout << "ERRO: " << errorString << endl;
		} else {
			int Narduinos;
			//Numero de Arduinos obtido
			if(sscanf(info, "Narduinos: %d", &Narduinos) == 1){
				cout << "Narduinos: " << Narduinos << endl;
			}
			//Calibração terminou
			if(strcmp(info, "Ready") == 0){
				calibration = true;
				cout << "Arduino[] Ready!" << endl;
			}
			if (ARDUINODEBUG){
				if(dataN > 0){
					cout << "Erro de comunicação, menos dados" << endl;
				}
			}
		}
	}
}

void Arduino::setNewInformationCallback(std::function<void(void)> fcn){
	newInformationCallback = fcn;
}

/* Retorna o valor máximo entre d1 e d2
 * Caso d1 = d2, retorna 0 */
double Arduino::getMax(double d1, double d2){
	if (d1 >= d2){
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

/* Calculo do valor do erro no instante k */
void Arduino::calcError(){
	if ((K >= 0) && (K < this->N)){
		this->e[this->K] = this->y[this->K] - this->ref[this->K];
	}
}

/* Calculo da energia gasta nos LEDs */
void Arduino::calcEnergy(){
	if ((K >= 0) && (K < this->N)){
		this->E[this->K] = this->E[this->getkPrevious(this->K)] 
						 + this->d[this->getkPrevious(this->K)]*(this->t[this->K]-this->t[getkPrevious(this->K)]);
	}
}

/* Calculo do erro de comforto com a luminsidade*/
void Arduino::calcComfortError(){
	if ((K >= 0) && (K < this->N)){
		// O comfort error acumulado é (N-1)/N*C(k-1) + 1/N*C(k)
		this->Cerror[this->K] = ((this->cycle-1)/this->cycle)*this->Cerror[getkPrevious(this->K)]
							  + 1/(double)cycle*getMax(this->e[this->K],0);
		
	}
}

/* Calculo da variancia de comforto */
void Arduino::calcComfortVariance(){
	double sum;
	if ((K >= 0) && (K < this->N)){
		sum = this->getAbs(this->y[this->K] 
		    - 2*this->y[this->getkPrevious(this->K)] 
			+ this->y[this->getkPrevious(this->getkPrevious(this->K))]);
		// O erro da variância acumulada é (N-1)/N*V(k-1) + 1/N*V(k)
		this->Verror[this->K] = ((cycle-1)/cycle)*this->Verror[getkPrevious(this->K)] 
							  + 1/cycle*sum/(this->T*this->T);
	}	
}

vector<double> Arduino::get_minute(vector<double> vec){
	int n = getkPrevious(K);
	int ciclos = 0;
	vector<double> new_vec(N);	
	double millis = t[n];

	do {
		new_vec[ciclos] = vec.at(n);
		ciclos++;
		n = getkPrevious(n);
	} while(millis <= t[n] + 1000 && ciclos <= N);

	new_vec.resize(ciclos+1);

	return new_vec;

}

void Arduino::reset(){
	th.interrupt();
	serial->Close();
}

//Cria ficheiros csv que podem ser lidos no MATLAB para fazer gráficos do relatório
bool Arduino::saveVectorsCSV(int i){
	bool ret[7];
	ret[0] = savetoCSV(this->t, "time" + to_string(i) + ".csv");
	ret[1] = savetoCSV(this->ref, "ref" + to_string(i) + ".csv");
	ret[2] = savetoCSV(this->y, "luminous" + to_string(i) + ".csv");
	ret[3] = savetoCSV(this->d, "duty" + to_string(i) + ".csv");
	ret[4] = savetoCSV(this->E, "Energy" + to_string(i) + ".csv");
	ret[5] = savetoCSV(this->Cerror, "Cerror" + to_string(i) + ".csv");
	ret[6] = savetoCSV(this->Verror, "Verror" + to_string(i) + ".csv");
	//Cria strings adequadas ao ficheiro
	for(int i = 0; i < 7; i++){
		cout << to_string(ret[i]) << endl;
	}
	return ret[0] || ret[1] || ret[2] || ret[3] || ret[4] || ret[5] || ret[6];
}

bool Arduino::isCalibration(){
	return this->calibration;
}


Arduino::~Arduino(){
	//Free dos vectores alocados
	this->t.clear();
	this->ref.clear();
	this->e.clear();
	this->u.clear();
	this->y.clear();
	this->d.clear();
	this->E.clear();
	this->Cerror.clear();
	this->Verror.clear();
}

bool Arduino::savetoCSV(vector<double> vec, string filename){
	ofstream file;
	string filepath;
	unsigned int i;
	//Open file
	filepath = "./data/" + filename;
	cout << filepath << endl;
	file.open(filepath);
	if(file.is_open()){
		for(i=0; i < vec.size(); i++){
			file << to_string(vec.at(i)) << endl;
		}
		file.close();
		return true;
	}
	return false;
}
