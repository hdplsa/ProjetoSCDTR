#include "LightSensor.h"
#include "LedPWM.h"
#include "LumItensity.h"

LightSensor *ls;
LedPWM *Ledp;
LumItensity *luminten;

const int ledPin = 9;
const int sensorPin = 5;

// funcoes auxiliares

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ls = new LightSensor(sensorPin);
  Ledp = new LedPWM(ledPin,0.085);
  luminten = new LumItensity(ls);
}

//Inicialização - Calibração
void nome_qualquer(){
  //Número de testes
  int N = 10;
  //Variáveis experimentais
  double y[N];
  double u[N];
  double us[N];
  //Variáveis Regressão
  int n;
  double sum = 0;
  double sumy = 0;
  double sums = 0;
  double sumyu = 0;
  double det, k, teta;
  //Recolha de dados para regressão linear
  for(n=0;n<=N;n=n+1){
    u[n] = 0.5*n;
    Ledp->setLedPWMVoltage(u[n]);
    delay(1000);
    y[n] = luminten->getLuminousItensity();
    /*Serial.print("y[n] = ");
    Serial.print(y[n]);
    Serial.print("\n");*/
  }
  //Regressão Linear (u - entrada, y - saída)
  for(n=0;n<=N;n=n+1){
    sum+=u[n];
    us[n]=u[n]*u[n];
    sums+=us[n];
    sumy+=y[n];
    sumyu+=u[n]*y[n];
  }
  det = 1/(N*sums - sum*sum);
  k = det*(N*sumyu - sum*sumy);
  teta = det*(-sum*sumyu + sums*sumy);

  Serial.print("k = ");
  Serial.print(k);
  Serial.print("\n");
  Serial.print("theta = ");
  Serial.print(teta);
  Serial.print("\n");
}

void reset_board_serial(void (*f())){

  int incoming;
  
  // Se ele tiver uma ligação serial, e receber 42,
  // Executa a função.
  if(Serial.available() > 0){

    incoming = Serial.read();

    if(incoming == 42){

      f();
      
    }  
}
}
void reset_led(){

  Ledp->setLedPWM(0); 

  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

  double R, lux;
  
  Ledp->setLedPWM(100);
  R = ls->getSensorResistance();
  lux = luminten->getLuminousItensity();

  //Serial.print("R = ");
  //Serial.print(R);
  //Serial.print("\n");
  Serial.print("lux = ");
  Serial.print(lux);
  Serial.print("\n");
  Serial.print("Energia = ");
  Serial.print(Ledp->calculateLedEnergyCycle(),4);
  Serial.print("\n");
  nome_qualquer();

  delay(50);

  reset_board_serial(reset_led);
    
  }
