#include "Arduino.h"

class Telemetry {
  public:
    static Telemetry& getInstance();
    void send(String data);
  private:
    Telemetry();
};
