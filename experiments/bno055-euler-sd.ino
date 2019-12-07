#include "SPI.h"
#include "SD.h"
#include "Wire.h"
#include "BNO055_API.h"

BNO055_API *imuSensor;
File imuLogs;

void setup()
{
  Serial.begin(9600);

  if (!SD.begin(4)) {
    Serial.println("SD initialization failed!");
    while(1);
  }
  Serial.println("SD initialization done.");

  Wire.begin();

  imuSensor = &(imuSensor->getInstance());
}

int i = 0;
void loop()
{
  String dataString = String(millis()) + "," + String(imuSensor->pitch()) + "," + String(imuSensor->yaw()) + "," + String(imuSensor->roll()) + ",";
  Serial.println(dataString);

  imuLogs = SD.open("imuLogs.txt", FILE_WRITE);
  if (imuLogs) {
    imuLogs.println(dataString);
    imuLogs.close();
  } else {
    Serial.println("Error opening logfile.");
  }

  delay(100);
}
