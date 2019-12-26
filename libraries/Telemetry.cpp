#include "Telemetry.h"

RH_RF95 Telemetry::rf95(8, 3);

Telemetry& Telemetry::getInstance() {
  static Telemetry instance;
  return instance;
}

void Telemetry::send(String data) {
  if (!init) {
    return;
  }

  if (Serial) {
    int messageLength = data.length();
    Serial.println("TELEMETRY (" + String(messageLength) + "): " + data);
  }

  char stream[data.length() + 1];
  data.toCharArray(stream, data.length() + 1);
  stream[data.length()] = '\0';
  rf95.send((uint8_t *)stream, sizeof(stream));
}

void Telemetry::setup() {
  if (!rf95.init()) {
    while(1);
  }
  init = true;
}

Telemetry::Telemetry() {}
