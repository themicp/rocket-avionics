#include "Arduino.h"
#include "SPI.h"
#include "RH_RF95.h"
#include "RHReliableDatagram.h"

class Telemetry {
  public:
    static Telemetry& getInstance();
    void send(String data);
    void setup();
    bool messageAvailable();
    String receiveMessage();

  private:
    bool init = false;
    static RH_RF95 rf95;
    static RHReliableDatagram rf_manager;
    Telemetry();
};
