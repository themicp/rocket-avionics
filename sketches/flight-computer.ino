#include "BMP280_API.h"
#include "BNO055_API.h"
#include "FSM.h"

BNO055_API imuSensor = BNO055_API::getInstance();
BMP280_API altimeter = BMP280_API::getInstance();
Telemetry telemetry = Telemetry::getInstance();
FSM *fsm;

void setup() {
  if (Serial) {
    Serial.begin(9600);
  }

  fsm = new FSM(&telemetry, &imuSensor, &altimeter);
}

void loop() {
  if (telemetry.messageAvailable()) {
    String message = telemetry.receiveMessage();
    if (message.substring(0, 3).equals("CMD")) {
      int event = message.substring(4).toInt();
      fsm->process_event((EVENT)event);
    }
  }

  if (Serial) {
    if (Serial.available()) {
      int a = Serial.parseInt();
      fsm->process_event((EVENT)a);
    }
  }

  fsm->runCurrentState();

  delay(10);
}
