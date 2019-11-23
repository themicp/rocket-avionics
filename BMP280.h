#include "PressureSensor.h"
#include "SparkFunBME280.h"

class BMP280: public PressureSensor {
  BME280 sensor;

  public:
    static BMP280& getInstance();
    float altitude();
    float pressure();

  private:
    BMP280();
};
