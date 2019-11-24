#include "Wire.h"
#include "BMP280_API.h"
#include "MPU9250_API.h"

BMP280_API *pressureSensor;
MPU9250_API *imuSensor;

void setup()
{
  Serial.begin(9600);
  Serial.println("Reading basic values from BME280");

  Wire.begin();

  pressureSensor = &(pressureSensor->getInstance());
  imuSensor = &(imuSensor->getInstance());
}

void loop()
{
  Serial.print("Pressure (Pa): ");
  Serial.print(pressureSensor->pressure());
  Serial.println();

  Serial.print("Altitude (m): ");
  Serial.print(pressureSensor->altitude());

  Serial.println();

  Serial.print("Pitch, Yaw, Roll (deg): ");
  Serial.print(imuSensor->pitch());
  Serial.print(", ");
  Serial.print(imuSensor->yaw());
  Serial.print(", ");
  Serial.print(imuSensor->roll());

  Serial.println();

  Serial.print("Acceleration X, Y, Z (mg): ");
  Serial.print(imuSensor->accelerationX());
  Serial.print(", ");
  Serial.print(imuSensor->accelerationY());
  Serial.print(", ");
  Serial.print(imuSensor->accelerationZ());

  Serial.println();

  Serial.print("Gyroscope X, Y, Z (deg/s): ");
  Serial.print(imuSensor->gyroX());
  Serial.print(", ");
  Serial.print(imuSensor->gyroY());
  Serial.print(", ");
  Serial.print(imuSensor->gyroZ());

  Serial.println();

  delay(1000);
}
