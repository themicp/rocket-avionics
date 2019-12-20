#include "Wire.h"
#include "Telemetry.h"
#include "IMU.h"
#include "Altimeter.h"

class FSM {
  IMU* imuSensor;
  Altimeter* altimeter;

  public:
    enum STATE {SETUP, IDLE, CALIBRATION, READY, ASCENDING, APOGEE, FTS, RECOVERING};
    enum EVENT {SETUP_COMPLETE, INIT_CALIBRATION, CALIBRATION_COMPLETE, LAUNCHED, APOGEE_DETECTED, TRIGGER_FTS, CHUTE_EJECTED};

    void process_event(FSM::EVENT event);
    void runCurrentState();

    FSM(IMU* imuSensor, Altimeter* altimeter);

  private:
    FSM::STATE state = SETUP;

    void onSetup();
    void onIDLE();
    void onCalibration();
    void onReady();
    void onAscending();
    void onApogee();
    void onFTS();
    void onRecovering();
};
