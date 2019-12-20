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


void BMP280_API::setup() {
  if (!sensor.begin()) {
    while(1); //Freeze
  }
}

BMP280_API::BMP280_API() {}
