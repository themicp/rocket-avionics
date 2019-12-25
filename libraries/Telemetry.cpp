#include "Telemetry.h"

RH_RF95 Telemetry::rf95(8, 3);

Telemetry& Telemetry::getInstance() {
  static Telemetry instance;
  return instance;
}

void Telemetry::send(String data) {
  if (Serial) {
    int messageLength = data.length();
    Serial.println("TELEMETRY (" + String(messageLength) + "): " + data);
  }
}

void Telemetry::setup() {
  if (!rf95.init()) {
    while(1);
  }
}

Telemetry::Telemetry() {}
