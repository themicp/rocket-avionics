#include "Arduino.h"
#include "Telemetry.h"

static Telemetry& Telemetry::getInstance() {
  static Telemetry instance;
  return instance;
}

void Telemetry::send(String data) {
  if (Serial) {
    Serial.println("TELEMETRY: " + data);
  }
}

Telemetry::Telemetry() {}
