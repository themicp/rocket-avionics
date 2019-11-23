#include "BMP280.h"

static BMP280& BMP280::getInstance() {
  static BMP280 instance;
  return instance;
}

float BMP280::altitude() {
  return sensor.readFloatAltitudeMeters();
}

float BMP280::pressure() {
  return sensor.readFloatPressure();
}

BMP280::BMP280() {
  if (sensor.beginI2C() == false) {
    Serial.println("The sensor did not respond. Please check wiring.");
    while(1); //Freeze
  }

  // Need to call it once, otherwise we receive wrong values
  // https://github.com/sparkfun/SparkFun_BME280_Arduino_Library/issues/39
  sensor.readTempF();
}
