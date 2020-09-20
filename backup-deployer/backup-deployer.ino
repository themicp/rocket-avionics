#include <Wire.h>
#include <stdint.h>

#define EJECTION_TIMEOUT 10000 //ms
#define ARM_COUNTDOWN 12000 //ms
#define IGNITER_PIN 14

unsigned long timer_start = 0;
unsigned long ignition_time = 0;
uint8_t state = 0;
bool armed = false;
bool countdown = false;

void setup() {
  pinMode(IGNITER_PIN, OUTPUT);
  digitalWrite(IGNITER_PIN, LOW);

  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void receiveEvent(int bytes) {
  state = (uint8_t)Wire.read();
}

void requestEvent() {
  uint8_t stream[2];
  stream[0] = state;
  stream[1] = 0;

  if (state == 1) {
    if (countdown) {
      if (timer_start > millis()) {
        stream[1] = (uint8_t)((timer_start - millis())/1000);
      }
    } else {
      stream[1] = (uint8_t)(ARM_COUNTDOWN/1000);
    }
  } else if (state == 2) {
    if (armed) {
      if (ignition_time > millis()) {
        stream[1] = (uint8_t)((ignition_time - millis())/1000);
      }
    } else {
      stream[1] = (uint8_t)(EJECTION_TIMEOUT/1000);
    }
  }

  Wire.write(stream, 2);
}

void loop() {
  if (state == 1) { // countdown
    if (!countdown) {
      timer_start = millis() + ARM_COUNTDOWN;
      countdown = true;
    }
    if (countdown && millis() >= timer_start) {
      state = 2;
    }
  } else if (state == 2) { // arm
    if (!armed) {
      ignition_time = millis() + EJECTION_TIMEOUT;
      armed = true;
    }
    if (armed && millis() >= ignition_time) {
      digitalWrite(IGNITER_PIN, HIGH);
    }
  } else if (state == 3) { // disarm
    armed = false;
    countdown = false;
    ignition_time = millis() + 1000;
    digitalWrite(IGNITER_PIN, LOW);
  }

  delay(100);
}
