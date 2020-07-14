#include "bmp3xx_api.h"
#include "lsm9ds1_api.h"
#include "fsm.h"
#include "mosfet_igniter.h"

LSM9DS1_API imu_sensor = LSM9DS1_API::getInstance();
BMP3XX_API altimeter = BMP3XX_API::getInstance();
Telemetry telemetry = Telemetry::getInstance();
MosfetIgniter igniter = MosfetIgniter::getInstance();
FSM *fsm;

#define SERIAL_DEBUG true
#define LOOP_FREQUENCY 10 // Hz

void setup() {
#if SERIAL_DEBUG
  Serial.begin(9600);
#endif

  fsm = new FSM(&telemetry, &imu_sensor, &altimeter, &igniter);
}

void loop() {
  int timerStart = millis();

  if (telemetry.messageAvailable()) {
    String message = telemetry.receiveMessage();
    if (message.substring(0, 3).equals("CMD")) {
      int event = message.substring(4).toInt();
      fsm->process_event((EVENT)event);
    }
  }

#if SERIAL_DEBUG
  if (Serial.available()) {
    int a = Serial.parseInt();
    fsm->process_event((EVENT)a);
  }
#endif

  fsm->runCurrentState();

  int delayTime = (1000/LOOP_FREQUENCY) - (millis() - timerStart);

  delay(max(0, delayTime));
}