#include "Arduino.h"
#include "SPI.h"
#include "RH_RF95.h"

class Telemetry {
  public:
    static Telemetry& getInstance();
    void send(String data);
    void setup();
  private:
    static RH_RF95 rf95;
    Telemetry();
};
