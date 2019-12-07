#include "Wire.h"
#include "BMP280_API.h"

BMP280_API *pressureSensor;

void setup()
{
  Serial.begin(9600);

  Wire.begin();

  pressureSensor = &(pressureSensor->getInstance());
}

void loop()
{
  Serial.print(pressureSensor->pressure());
  Serial.print(",");
  Serial.print(pressureSensor->altitude());
  Serial.println();

  delay(100);
}
