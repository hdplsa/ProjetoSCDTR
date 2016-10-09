#ifndef LIGHTCONTROLLER_H_INCLUDED
#define LIGHTCONTROLLER_H_INCLUDED

#include "LumIntensity.h"
#include "LedPWM.h"

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
  LumIntensity *lit;
  LedPWM *ledp;
  //Variáveis do modelo
  double k, teta;
  
  public:
    LightController(double Kp,double Ti,double Td);
    ~LightController();
  
  private:
    void calibrateLumVoltage();
  
};

#endif
