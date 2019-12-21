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

void Telemetry::sendValues(float *values, int N) {
  String message = "";
  for (int i = 0; i < N; ++i) {
    message += String(values[i]);
    if (i != N - 1) {
      message += ",";
    }
  }

  send("VALUES: " + message);
}

Telemetry::Telemetry() {}
