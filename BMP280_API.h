#include "PressureSensor.h"
#include "SparkFunBME280.h"

class BMP280_API: public PressureSensor {
  BME280 sensor;

  public:
    static BMP280_API& getInstance();
    float altitude();
    float pressure();

  private:
    BMP280_API();
};
