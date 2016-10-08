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
  luminten = new LumItensity(ls);
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
  
  //Ledp->setLedPWM(100);
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

  delay(50);

  reset_board_serial(reset_led);
    
  }
