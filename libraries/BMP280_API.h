#include "Altimeter.h"
#include "Adafruit_BMP280.h"


class BMP280_API: public Altimeter {
  static Adafruit_BMP280 sensor;

  public:
    static BMP280_API& getInstance();
    float altitude();
    float pressure();

    void setup();

  private:
    BMP280_API();
};
