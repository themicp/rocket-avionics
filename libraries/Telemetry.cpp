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

bool Telemetry::messageAvailable() {
  return rf95.available();
}

String Telemetry::receiveMessage() {
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  rf95.recv(buf, &len);
  return String((char*)buf);
}

Telemetry::Telemetry() {}
