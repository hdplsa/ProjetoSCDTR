#include "Arduino.h"

Arduino::Arduino(int N_, string port) : N(N_), ref(N_,0), e(N_,0), u(N_,0), y(N_,0), d(N_,0), E(N_,0), Cerror(N_,0), Verror(N_,0) {

	//Valores iniciais
	this->N = N;
	this->k = 0;

	this->o = false;

	// Abre a porta serial
	serial->Begin(115200, (const char*) port.c_str());

	// Atribui os callbacks
	serial->set_Readcallback(std::bind(&Arduino::receiveInformation, this, std::placeholders::_1));

	// Começa a leitura do serial
	serial->Read_ln();
	
	// Definição de comprimentos dos vectores
	this->ref->resize(N);
	this->e->resize(N);
	this->u->resize(N);
	this->y->resize(N);
	this->d->resize(N);
	this->E->resize(N);
	this->Cerror->resize(N);
	this->Verror->resize(N);
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

void Arduino::send(string str){

	serial->Write(str);

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

/* Calculo do valor do erro no instante k */
void Arduino::calcError(){
	if ((k >= 0) && (k < this->N)){
		this->e[this->k] = this->y[this->k] - this->ref[this->k];
	}
}

/* Calculo da energia gasta nos LEDs */
void Arduino::calcEnergy(){
	if ((k >= 0) && (k < this->N)){
		this->E[this->k] = this->E[this->getkPrevious(this->k)] + this->d[this->getkPrevious(this->k)]*this->T;
	}
}

/* Calculo do erro de comforto com a luminsidade*/
void Arduino::calcComfortError(){
	if ((k >= 0) && (k < this->N)){
		this->Cerror[this->k] = ((this->t-1)/this->t)*getMax(this->e[this->k],0);
	}
}

/* Calculo da variancia de comforto */
void Arduino::calcComfortVariance(){
	double sum1, sum2;
	double sum; // (?)
	if ((k >= 0) && (k < this->N)){
		sum = this->getAbs(this->y[this->k]-2*this->y[this->getkPrevious(this->k)]+this->y[this->getkPrevious(this->getkPrevious(this->k))]);
		this->Verror[this->k] = ((this->t-1)/this->t)*sum/(this->T*this->T);
	}
}

/* Função chamada ciclicamente para receber e guardar dados
 * vindos dos Arduinos */
void Arduino::receiveInformation(string info){

	float y;
	float e;
	float u;
	long t;

	// Estrai os dados da string
	sscanf(info.c_str(), "y = %f;e = %f; u = %f; t = %ld\n", &y, &e, &u, &t);

	// Guarda os dados no objeto
	this->y[k] = y;
	this->e[k] = e;
	this->u[k] = u;
		// Guardar o tempo tbm?

	//Calcula erro
	this->calcError();
	
	//Avança da o instante seguinte
	this->k = this->getkNext(this->k);
	this->t = this->t + 1;

	// Manda ler a próxima linha
	serial->Read_ln();
}

Arduino::~Arduino(){
	
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