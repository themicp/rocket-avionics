#include "Wire.h"
#include "Telemetry.h"
#include "IMU.h"
#include "Altimeter.h"

enum class STATE {INVALID_STATE, SETUP, IDLE, CALIBRATION, READY, ASCENDING, APOGEE_TIMEOUT, APOGEE, FTS, RECOVERING, Count};

enum class EVENT {SETUP_COMPLETE, INIT_CALIBRATION, CALIBRATION_COMPLETE, LAUNCHED, APOGEE_TIMER_TIMEOUT, APOGEE_DETECTED, TRIGGER_FTS, CHUTE_EJECTED, Count};

class Transition {
  public:
    STATE src_state;
    EVENT event;
    STATE dst_state;

    Transition(STATE src_state, EVENT event, STATE dst_state)
      : src_state(src_state)
      , event(event)
      , dst_state(dst_state)
    {}
};

class FSM {
  Telemetry* telemetry;
  IMU* imuSensor;
  Altimeter* altimeter;
  int launchTime;

  public:
    template<size_t N>
    void register_state_transitions(Transition (&transitions)[N]);

    void process_event(EVENT event);
    void runCurrentState();

    FSM(Telemetry* telemetry, IMU* imuSensor, Altimeter* altimeter);

  private:
    STATE state = STATE::SETUP;
    STATE state_transitions[(int)STATE::Count][(int)EVENT::Count];

    void onSetup();
    void onIDLE();
    void onCalibration();
    void onReady();
    void onAscending();
    void onApogeeTimeout();
    void onApogee();
    void onFTS();
    void onRecovering();
};
