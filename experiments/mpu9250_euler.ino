#include "Wire.h"
#include "BMP280_API.h"
#include "MPU9250_API.h"

BMP280_API *pressureSensor;
MPU9250_API *imuSensor;

void setup()
{
  Serial.begin(9600);

  Wire.begin();

  pressureSensor = &(pressureSensor->getInstance());
  imuSensor = &(imuSensor->getInstance());
}

void loop()
{
  Serial.print(imuSensor->pitch());
  Serial.print(",");
  Serial.print(imuSensor->yaw());
  Serial.print(",");
  Serial.print(imuSensor->roll());
  Serial.println();

  delay(50);
}
