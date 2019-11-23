#include "Wire.h"
#include "BMP280.h"

BMP280 *pSensor;

void setup()
{
  Serial.begin(9600);
  Serial.println("Reading basic values from BME280");

  Wire.begin();

  pSensor = &(pSensor->getInstance());
}

void loop()
{
  Serial.print(" Pressure: ");
  Serial.print(pSensor->pressure());

  Serial.print(" AltM: ");
  Serial.print(pSensor->altitude());

  Serial.println();

  delay(50);
}
