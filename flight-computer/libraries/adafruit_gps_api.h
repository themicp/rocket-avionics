#include <Adafruit_GPS.h>
#include "gps.h"

class Adafruit_GPS_API: public GPS {
  public:
    static Adafruit_GPS_API& getInstance();

    void setup();
    bool fix();
    String latitude();
    String longitude();
    float altitude();
    int satellites();

  private:
    Adafruit_GPS_API();
    static Adafruit_GPS receiver;
    unsigned long last_read;

    void readData();
};
