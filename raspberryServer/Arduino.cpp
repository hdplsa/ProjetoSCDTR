#include "Arduino.h"

Arduino::Arduino(int N_, string port) : N(N_), ref(N_,0), e(N_,0), u(N_,0), y(N_,0), d(N_,0), t(N_), E(N_,0), Cerror(N_,0), Verror(N_,0) {

	//Valores iniciais
	this->K = 0;
	this->o = false;

	// Abre a porta serial
	serial = new Serial();
	serial->Begin(115200, port.c_str());

	// Atribui os callbacks
	serial->set_Readcallback(std::bind(&Arduino::receiveInformation, this, std::placeholders::_1));

	// Começa a leitura do serial
	serial->Read_ln();
	
}

/* Implementação dos indices do vector em anel
 * Assim, é necessário determinar os indices 
 * seguintes e anteriores dos extremos do vector */
int Arduino::getkNext(int k){
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

double Arduino::getEnergy(){
	return this->E[this->K];
}

double Arduino::getEnergy(int k){
	return this->E[k];
}

double Arduino::getComfortError(){
	return this->Cerror[this->K];
}

double Arduino::getComfortError(int k){
	return this->Cerror[k];
}

double Arduino::getComfortVariance(){
	return this->Verror[this->K];
}

double Arduino::getComfortVariance(int k){
	return this->Verror[k];
}

double Arduino::getIlluminance(){
	return this->y[this->K];
}

double Arduino::getIlluminance(int k){
	return this->y[k];
}

vector<double> Arduino::getLastMinuteIlluminance(){
	vector<double> sum = get_minute(y);

	return sum;
}

double Arduino::getDuty(){
	return this->d[this->K];
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

void Arduino::setOccupancy(bool value){
	this->o = value;
}

double Arduino::getRef(){
	return this->ref[K];
}

void Arduino::send(string str){

	serial->Write(str);

}

double Arduino::getPower(){
	throw "Not implemented";
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

void Arduino::ledON(float V){

	int valor = (int)(V*255/5);

	ledON(valor);

}

void Arduino::ledOFF(){

	ledON(0);

}

/* Função chamada ciclicamente para receber e guardar dados
 * vindos dos Arduinos */
void Arduino::receiveInformation(string info){

	float y;
	float e;
	float u;
	long t_; // Está aqui para ser removido <----------

	// Estrai os dados da string
	sscanf(info.c_str(), "y = %f;e = %f; u = %f; t = %ld\n", &y, &e, &u, &t_);

	// Guarda os dados no objeto
	this->y[K] = y;
	this->e[K] = e;
	this->u[K] = u;
	this->t[K] = boost::posix_time::microsec_clock::local_time();
	
	//Avança da o instante seguinte
	this->K = this->getkNext(this->K);
	this->cycle++;

	newInformationCallback();

	// Manda ler a próxima linha
	serial->Read_ln();
}

void Arduino::setNewInformationCallback(std::function<void(void)> fcn){
	newInformationCallback = fcn;
}

/* Retorna o valor máximo entre d1 e d2
 * Caso d1 = d2, retorna 0 */
double Arduino::getMax(double d1, double d2){
	if (d1 > d2){
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
	boost::posix_time::time_duration td;
	if ((K >= 0) && (K < this->N)){
		td = this->t[K] - this->t[getkPrevious(K)];
		this->E[this->K] = this->E[this->getkPrevious(this->K)] 
			             + this->d[this->getkPrevious(this->K)]*td.total_milliseconds()*1e-3;
	}
}

/* Calculo do erro de comforto com a luminsidade*/
void Arduino::calcComfortError(){
	if ((K >= 0) && (K < this->N)){
		// O comfort error acumulado é (N-1)/N*C(k-1) + 1/N*C(k)
		this->Cerror[this->K] = ((this->cycle-1)/this->cycle)*this->Cerror[getkPrevious(this->K)]
							  + 1/cycle*getMax(this->e[this->K],0);
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
	int n = K;
	int ciclos = 0;
	vector<double> new_vec(N);

	boost::posix_time::time_duration td;

	do{
		td = t[K] - t[getkPrevious(n)];
		new_vec.push_back(vec.at(n));
		n = getkPrevious(n);
		ciclos++;

	} while(td.total_seconds() < 60 && ciclos < N);

	return new_vec;

}

void Arduino::reset(){
	throw "Not implemented";
}


Arduino::~Arduino(){
	
}
