#include "Arduino.h"

class Telemetry {
  public:
    static Telemetry& getInstance();
    void send(String data);
    void sendValues(float *values, int N);
  private:
    Telemetry();
};
