#ifndef LIGHTCONTROLLER_H_INCLUDED
#define LIGHTCONTROLLER_H_INCLUDED

#include "LedPWM.h"
#include "LightSensor.h"

const double Vcc = 5.0;

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
  double e[2];
  double y;
  double u[2];
  double ui_ant;
  double ud_ant;
  double sat_up, sat_down; 

  //deadzone
  const double deadzone = 2;

  // Windup
  double Kw;
  double windup[2];
  
  public:
    LightController(int ledPin, int sensorPin);
    void calibrateLumVoltage();
    void lightoff();
    void lighton();
    void setT(double T);
    void setY(double y);
    void setRef(int ref);
    void setU(double u);
    void setSaturation(double sat_up);
    double getK();
    double getTeta();
    double getT();
    double getControlVariable();
    double getY();
    double getError();
    double calcController(); 
    void LEDInputControlVariable();
    ~LightController();
  
  private:
    double getSensorY();
    double calcErro();
    double calcFeedForward();
    double calcDeadzone(double e);
    double calcPController();
    double calcPIController();
    double calcPDController();   
  
};

#endif
