#ifndef LIGHTCONTROLLER_H_INCLUDED
#define LIGHTCONTROLLER_H_INCLUDED

#include "LedPWM.h"
#include "LightSensor.h"

/* class que pretende implementar um controlador PID
 * Como input do controlador está o erro de luminosidade medido
 * pela diferença entre a referência pretendida e o feedback
 * C(s) = Kp[1 + 1/(Ti*s) + s*Td]
 * Controlador retirado dos slides de controlo
 */

class LightController{
  //Parâmetros do controlador
  double Kp;
  double Ti;
  double Td;
  //Depêndencias do feedback
  LightSensor *ls;
  LedPWM *ledp;
  //Variáveis do modelo
  double k, teta;
  
  public:
    LightController(int ledPin, int sensorPin, double Kp,double Ti,double Td);
    ~LightController();
    void calibrateLumVoltage();
    void lightoff();
  
  private:
    
  
};

#endif
