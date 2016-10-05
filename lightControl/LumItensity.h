#ifndef LUM_H_INCLUDED
#define LUM_H_INCLUDED

class LumItensity{
  //Resistência do sensor
  double Rs;
  //Intensidade luminosa
  double lumIt;

  public:
    LumItensity();
    void setSensorResistance(double Rs);
    double getLuminousItensity();
    ~LumItensity();
    
  private:
};


#endif
