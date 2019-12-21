#include "BMP280_API.h"

#define BMP_SCK  (13)
#define BMP_MISO (12) // SD0
#define BMP_MOSI (11) // SD1
#define BMP_CS   (10)

Adafruit_BMP280 BMP280_API::sensor;

static BMP280_API& BMP280_API::getInstance() {
  static BMP280_API instance;
  return instance;
}

float BMP280_API::altitude() {
  return sensor.readAltitude();
}

float BMP280_API::pressure() {
  return sensor.readPressure();
}

float BMP280_API::agl() {
  return altitude() - groundLevel;
}

void BMP280_API::setGroundLevel() {
  groundLevel = sensor.readAltitude();
}

float BMP280_API::getGroundLevel() {
  return groundLevel;
}

void BMP280_API::setup() {
  if (!sensor.begin()) {
    while(1); //Freeze
  }
}

void BMP280_API::calibrate() {
  sensor.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16, // TODO: verify that value
    Adafruit_BMP280::STANDBY_MS_1);

  setGroundLevel();
}

BMP280_API::BMP280_API() {}
