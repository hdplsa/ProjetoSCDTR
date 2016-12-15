#ifndef LIGHTCONTROLLER_H_INCLUDED
#define LIGHTCONTROLLER_H_INCLUDED

#include "LedPWM.h"
#include "LightSensor.h"

static const double Vcc = 5.0;

/* class que pretende implementar um controlador PID
 * Como input do controlador está o erro de luminosidade medido
 * pela diferença entre a referência pretendida e o feedback
 * C(s) = Kp + Ki/s + Kd*s
 * Controlador retirado dos slides de controlo
 */

class LightController{
    
    //Parâmetros do controlador
    const double Kp = 0.008;
    const double Ki = 0.1;
    const double Kd = 0.000;
    
    //Depêndencias do feedback
    LightSensor *ls;
    LedPWM *ledp;
    
    //Número de Arduinos
    int Narduino;

    //Index do Arduino
    int index;

    //Parametros de Calibração
    double *k;
    double theta;
    
    //Input/Output
    double T = 0.02;
    double ref = 0;
    //Valores de erro em dois instantes temporais
    double e[2] = {0,0};
    double y = 0;
    double *u;
    double u_ant = 0;
    //Valores de input do PID
    double u_pid = 0;
    double ui_ant = 0;
    double ud_ant = 0;
    //Valores de saturação
    double sat_up = 5.0;
    double sat_down = 0;
    volatile int ffflag = 0;
    
    //deadzone
    const double deadzone = 2;
    
    // Windup
    const double Kw = 10;
    double windup[2] = {0,0};
    
    
public:
    LightController(int Narduino, int ledPin, int sensorPin);
    void setRef(int ref);
    void setZeroUvec();
    void setUn(double Un, int n);
    void setUnFromdc(int dcn, int n);
    void setLedU(double u);
    void setSaturation(double sat_up, double sat_down);
    void setK(double *k);
    void setTheta(double theta);
    double getT();
    double getY();
    double getAverageY(const int N);
    double getOwnU();
    int getOwnDutyCycle();
    double getError();
    double calcController();
    void LEDInputControlVariable();
    void SetIndex(int index);
    ~LightController();
    
private:
    double getMax(double d1, double d2);
    double getSensorY();
    double calcErro();
    double calcFeedForward();
    double calcDeadzone(double e);
    double calcPController();
    double calcPIController();
    double calcPDController();
    double calSumOtherKus();   
};

#endif
