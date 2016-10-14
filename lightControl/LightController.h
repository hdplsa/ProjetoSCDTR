#ifndef LIGHTCONTROLLER_H_INCLUDED
#define LIGHTCONTROLLER_H_INCLUDED

#include "LedPWM.h"
#include "LightSensor.h"

/* class que pretende implementar um controlador PID
 * Como input do controlador está o erro de luminosidade medido
 * pela diferença entre a referência pretendida e o feedback
 * C(s) = Kp + Ki/s + Kd*s
 * Controlador retirado dos slides de controlo
 */

class LightController{
  //Parâmetros do controlador
  double Kp;
  double Ki;
  double Kd;
  //Depêndencias do feedback
  LightSensor *ls;
  LedPWM *ledp;
  //Variáveis do modelo
  double k, teta;
  //Input/Output
  double T;
  double ref;
  double e[3];
  double y;
  double u[2];
  
  public:
    LightController(int ledPin, int sensorPin, double Kp,double Ki,double Kd);
    void calibrateLumVoltage();
    void lightoff();
    void lighton();
    void setT(double T);
    void setY(double y);
    void setRef(double ref);
    void setU(double u);
    double getK();
    double getTeta();
    double getControlVariable();
    void LEDInputControlVariable();
    ~LightController();
  
  private:
    double calcErro();
    double calcPController();
    double calcPIController();
    double calcPDController(); 
    double calcController();   
  
};

#endif
