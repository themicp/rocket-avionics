#include "Telemetry.h"

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
#define SERIAL_DEBUG false
#define SD_LOGS true

// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
RH_RF95 Telemetry::rf95(8, 3);
RHReliableDatagram Telemetry::rf_manager(Telemetry::rf95, SERVER_ADDRESS);

Telemetry& Telemetry::getInstance() {
  static Telemetry instance;
  return instance;
}

void Telemetry::send(String data) {
  if (!init) {
    return;
  }

#if SERIAL_DEBUG
    int messageLength = data.length();
    Serial.println("TELEMETRY (" + String(messageLength) + "): " + data);
#endif

  char stream[data.length() + 1];
  data.toCharArray(stream, data.length() + 1);
  stream[data.length()] = '\0';

#if SD_LOGS
  logsFile = SD.open(logsFilename, FILE_WRITE);
  if (logsFile) {
    logsFile.println(stream);
    logsFile.close();
  }
#endif

  rf95.send((uint8_t *)stream, sizeof(stream));
  rf95.waitPacketSent();
  rf95.setModeRx(); // continue listening
}

void Telemetry::setup() {
  if (!rf_manager.init()) {
    while(1);
  }

  rf95.setSignalBandwidth(500000);
  rf95.setSpreadingFactor(8);

#if SD_LOGS
  if (!SD.begin(10)) {
    while(1);
  }

  if (SD.exists(logsFilename)) {
    SD.remove(logsFilename);
  }

  logsFile = SD.open(logsFilename, FILE_WRITE);
  if (!logsFile) { // test if we can open the file
    while(1);
  } else {
    logsFile.close();
  }
#endif

  init = true;
}

bool Telemetry::messageAvailable() {
  return rf_manager.available();
}

String Telemetry::receiveMessage() {
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  uint8_t from;

  rf_manager.recvfromAck(buf, &len, &from);
  return String((char*)buf);
}

Telemetry::Telemetry() {}
