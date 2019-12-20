#pragma once //(or use a guard symbol)

class Altimeter {
  public:
    virtual float altitude(); // meters
    virtual float pressure(); // pascal

    virtual void setup();
};
