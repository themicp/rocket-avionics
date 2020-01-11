#include "FSM.h"
#include "memoryUsage.h"

#define LAUNCH_AGL_THRESHOLD 5 // meters
#define APOGEE_AGL_DIFF_THRESHOLD 5 // meters
#define LAUNCH_ACCELERATION_THRESHOLD 3 // g
#define TIME_TO_APOGEE 10 // s
#define GRAVITY 9.81 // m/s^2 -- update on other planets
#define VBATPIN A7

// TODO: add transition from ASCENDING to READY (for false positive launche detection)

String state_to_str(STATE state) {
  String names[] = {"INVALID_STATE", "SETUP", "IDLE", "CALIBRATION", "READY", "ASCENDING", "APOGEE_TIMEOUT", "APOGEE", "FTS", "RECOVERING"};
  if ((int)state >= (int)STATE::Count or (int)state < 0) {
    return "Unknown";
  }
  return names[(int)state];
}

String event_to_str(EVENT event) {
  String names[] = {"SETUP_COMPLETE", "INIT_CALIBRATION", "CALIBRATION_COMPLETE", "LAUNCHED", "APOGEE_TIMER_TIMEOUT", "APOGEE_DETECTED", "TRIGGER_FTS", "CHUTE_EJECTED"};
  if ((int)event >= (int)EVENT::Count or (int)event < 0) {
    return "Unknown";
  }
  return names[(int)event];
}

FSM::FSM(Telemetry* telemetry, IMU* imuSensor, Altimeter* altimeter)
  : telemetry(telemetry)
  , imuSensor(imuSensor)
  , altimeter(altimeter)
{
  Transition flight_state_transitions[] = {
    Transition(STATE::SETUP, EVENT::SETUP_COMPLETE, STATE::IDLE),
    Transition(STATE::IDLE, EVENT::INIT_CALIBRATION, STATE::CALIBRATION),
    Transition(STATE::READY, EVENT::INIT_CALIBRATION, STATE::CALIBRATION),
    Transition(STATE::CALIBRATION, EVENT::CALIBRATION_COMPLETE, STATE::READY),
    Transition(STATE::READY, EVENT::LAUNCHED, STATE::ASCENDING),
    Transition(STATE::ASCENDING, EVENT::APOGEE_TIMER_TIMEOUT, STATE::APOGEE_TIMEOUT),
    Transition(STATE::ASCENDING, EVENT::APOGEE_DETECTED, STATE::APOGEE),
    Transition(STATE::APOGEE_TIMEOUT, EVENT::APOGEE_DETECTED, STATE::APOGEE),
    Transition(STATE::APOGEE, EVENT::CHUTE_EJECTED, STATE::RECOVERING),
    Transition(STATE::FTS, EVENT::CHUTE_EJECTED, STATE::RECOVERING),

    // FTS from all states
    Transition(STATE::SETUP, EVENT::TRIGGER_FTS, STATE::FTS),
    Transition(STATE::IDLE, EVENT::TRIGGER_FTS, STATE::FTS),
    Transition(STATE::CALIBRATION, EVENT::TRIGGER_FTS, STATE::FTS),
    Transition(STATE::READY, EVENT::TRIGGER_FTS, STATE::FTS),
    Transition(STATE::ASCENDING, EVENT::TRIGGER_FTS, STATE::FTS),
    Transition(STATE::APOGEE_TIMEOUT, EVENT::TRIGGER_FTS, STATE::FTS),
    Transition(STATE::APOGEE, EVENT::TRIGGER_FTS, STATE::FTS),
    Transition(STATE::RECOVERING, EVENT::TRIGGER_FTS, STATE::FTS)
  };

  register_state_transitions(flight_state_transitions);
}

template<size_t N>
void FSM::register_state_transitions(Transition (&transitions)[N]) {
  for (int i = 0; i < (int)STATE::Count; ++i) {
    for (int j = 0; j < (int)EVENT::Count; ++j) {
      state_transitions[i][j] = STATE::INVALID_STATE;
    }
  }

  for (int i = 0; i < N; ++i) {
    state_transitions[(int)transitions[i].src_state][(int)transitions[i].event] = transitions[i].dst_state;
  }
}

void FSM::process_event(EVENT event) {
  telemetry->send("FSM Event: " + event_to_str(event));

  if (state_transitions[(int)state][(int)event] != STATE::INVALID_STATE) {
    state = state_transitions[(int)state][(int)event];
  } else {
    telemetry->send("Illegal state transition from state " + state_to_str(state) + " with event " + event_to_str(event));
  }

  if (event == EVENT::LAUNCHED) {
    // TODO: not the best place to do that
    launchTime = millis();
  }
}

void FSM::onSetup() {
  telemetry->setup();
  telemetry->send("Hello!");

  Wire.begin();
  telemetry->send("Wire begin");

  telemetry->send("Setting up IMU..");
  imuSensor->setup();
  telemetry->send("IMU setup complete.");

  telemetry->send("Setting up Altimeter..");
  altimeter->setup();
  telemetry->send("Altimeter setup complete.");

  process_event(EVENT::SETUP_COMPLETE);
}

void FSM::onIDLE() {}

void FSM::onCalibration() {
  telemetry->send("Altimeter: calibrating..");
  altimeter->calibrate();
  telemetry->send("Altimeter: ground level set to " + String(altimeter->getGroundLevel()) + "m");

  telemetry->send("IMU: calibrating..");
  imuSensor->calibrate();
  telemetry->send("IMU: calibrated.");

  process_event(EVENT::CALIBRATION_COMPLETE);
}

void FSM::onReady() {
  if (altimeter->agl() > LAUNCH_AGL_THRESHOLD or
      imuSensor->accelerationX() / GRAVITY > LAUNCH_ACCELERATION_THRESHOLD) {
    process_event(EVENT::LAUNCHED);
  }
}

void FSM::onAscending() {
  float agl = altimeter->agl();
  if (agl > maxAgl) {
    maxAgl = agl;
  }

  if (maxAgl - agl >= APOGEE_AGL_DIFF_THRESHOLD) {
    process_event(EVENT::APOGEE_DETECTED);
  } else if (millis() - launchTime > TIME_TO_APOGEE * 1000) {
    process_event(EVENT::APOGEE_TIMER_TIMEOUT);
  }
  // TODO: check IMU ?
}

void FSM::onApogeeTimeout() {
  process_event(EVENT::APOGEE_DETECTED);
}

void FSM::onApogee() {
  process_event(EVENT::CHUTE_EJECTED);
  // TODO: trigger and detect ejection charge
}

void FSM::onFTS() {
  process_event(EVENT::CHUTE_EJECTED);
  // TODO: trigger and detect ejection charge
}

void FSM::onRecovering() {
  // TODO: celebrate
}

String internalBoardStatus() {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;
  measuredvbat *= 3.3;
  measuredvbat /= 1024;

  return
    String(millis()) + "," +
    String(freeMemory()/1000) + "," +
    String(measuredvbat);
}

void FSM::runCurrentState() {
  String message = "RAW:" + internalBoardStatus() + "," + state_to_str(state);
  if (state != STATE::SETUP and state != STATE::IDLE and state != STATE::CALIBRATION) {
    message += "," +
      String(altimeter->agl()) + "," +
      String(imuSensor->accelerationX()) + "," +
      String(imuSensor->accelerationY()) + "," +
      String(imuSensor->accelerationZ()) + "," +
      String(imuSensor->gyroX()) + "," +
      String(imuSensor->gyroY()) + "," +
      String(imuSensor->gyroZ());
  }
  telemetry->send(message);

  switch (state) {
    case STATE::SETUP:
      onSetup();
      break;
    case STATE::IDLE:
      onIDLE();
      break;
    case STATE::CALIBRATION:
      onCalibration();
      break;
    case STATE::READY:
      onReady();
      break;
    case STATE::ASCENDING:
      onAscending();
      break;
    case STATE::APOGEE:
      onApogee();
      break;
    case STATE::APOGEE_TIMEOUT:
      onApogeeTimeout();
      break;
    case STATE::FTS:
      onFTS();
      break;
    case STATE::RECOVERING:
      onRecovering();
      break;
  }
}
