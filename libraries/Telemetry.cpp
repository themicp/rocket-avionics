#include "Telemetry.h"

static Telemetry& Telemetry::getInstance() {
  static Telemetry instance;
  return instance;
}

void Telemetry::send(String data) {
  if (Serial) {
    int messageLength = data.length();
    Serial.println("TELEMETRY (" + String(messageLength) + "): " + data);
  }
}

Telemetry::Telemetry() {}
