#include "BMP280_API.h"

static BMP280_API& BMP280_API::getInstance() {
  static BMP280_API instance;
  return instance;
}

float BMP280_API::altitude() {
  return sensor.readFloatAltitudeMeters();
}

float BMP280_API::pressure() {
  return sensor.readFloatPressure();
}

BMP280_API::BMP280_API() {
  if (sensor.beginI2C() == false) {
    Serial.println("The sensor did not respond. Please check wiring.");
    while(1); //Freeze
  }

  // Need to call it once, otherwise we receive wrong values
  // https://github.com/sparkfun/SparkFun_BME280_Arduino_Library/issues/39
  sensor.readTempF();
}
