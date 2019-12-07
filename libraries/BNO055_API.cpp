#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BNO055_API.h"

// #define BNO055_SAMPLERATE_DELAY_MS (100)

static BNO055_API& BNO055_API::getInstance() {
  static BNO055_API instance;
  return instance;
}

Adafruit_BNO055 BNO055_API::sensor = Adafruit_BNO055(-1, 0x28);

void BNO055_API::readSensorData() {
  // TODO: fix units
  euler = sensor.getVector(Adafruit_BNO055::VECTOR_EULER);
  acc = sensor.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  gyro = sensor.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  mag = sensor.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
}

float BNO055_API::pitch() {
  readSensorData();
  return euler.y();
}

float BNO055_API::yaw() {
  readSensorData();
  return euler.x();
}

float BNO055_API::roll() {
  readSensorData();
  return euler.z();
}

float BNO055_API::accelerationX() {
  readSensorData();
  return acc.x();
}

float BNO055_API::accelerationY() {
  readSensorData();
  return acc.y();
}

float BNO055_API::accelerationZ() {
  readSensorData();
  return acc.x();
}

float BNO055_API::gyroX() {
  readSensorData();
  return gyro.x();
}

float BNO055_API::gyroY() {
  readSensorData();
  return gyro.y();
}

float BNO055_API::gyroZ() {
  readSensorData();
  return gyro.z();
}

float BNO055_API::magX() {
  readSensorData();
  return mag.x();
}

float BNO055_API::magY() {
  readSensorData();
  return mag.y();
}

float BNO055_API::magZ() {
  readSensorData();
  return mag.z();
}

BNO055_API::BNO055_API() {
  if(!sensor.begin()) {
    Serial.print("No BNO055 detected. Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  // TODO: do we need this?
  sensor.setExtCrystalUse(true);
}
