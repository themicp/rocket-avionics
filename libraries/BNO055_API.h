#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "IMU.h"

class BNO055_API: public IMU {
  static Adafruit_BNO055 sensor;

  public:
    static BNO055_API& getInstance();
    float pitch();
    float yaw();
    float roll();

    float accelerationX();
    float accelerationY();
    float accelerationZ();

    float gyroX();
    float gyroY();
    float gyroZ();

    float magX();
    float magY();
    float magZ();

    void setup();

  private:
    BNO055_API();
    void readSensorData();
    imu::Vector<3> euler, acc, gyro, mag;
};
