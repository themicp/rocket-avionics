#include "FSM.h"
#include "memoryUsage.h"

Telemetry telemetry = Telemetry::getInstance();

#define LAUNCH_AGL_THRESHOLD 50 // meters
#define LAUNCH_ACCELERATION_THRESHOLD 3 // g
#define TIME_TO_APOGEE 10 // s
#define GRAVITY 9.81 // m/s^2 -- update on other planets

// TODO: add transition from ASCENDING to READY (for false positive launche detection)

FSM::FSM(IMU* imuSensor, Altimeter* altimeter) {
  this->imuSensor = imuSensor;
  this->altimeter = altimeter;
}

void FSM::process_event(FSM::EVENT event) {
  telemetry.send("FSM Event: " + String(event));

  STATE previousState = state;
  switch (event) {
    case SETUP_COMPLETE:
      if (state == SETUP) {
        state = IDLE;
      }
      break;
    case INIT_CALIBRATION:
      if (state == IDLE or state == READY) {
        state = CALIBRATION;
      }
      break;
    case CALIBRATION_COMPLETE:
      if (state == CALIBRATION) {
        state = READY;
      }
      break;
    case LAUNCHED:
      if (state == READY) {
        state = ASCENDING;
      }
      // TODO: FTS otherwise?
      break;
    case APOGEE_TIMER_TIMEOUT:
      if (state == ASCENDING) {
        state = APOGEE_TIMEOUT;
      }
      break;
    case APOGEE_DETECTED:
      if (state == ASCENDING) {
        state = APOGEE;
      }
      break;
    case TRIGGER_FTS:
      // Allow FTS from any state.
      state = FTS;
      break;
    case CHUTE_EJECTED:
      if (state == APOGEE or state == FTS) {
        state = RECOVERING;
      }
      break;
    default:
      telemetry.send("Invalid event: " + String(event));
  }

  if (previousState == state) {
    telemetry.send("Illegal state transition from state " + String(state) + " with event " + String(event));
  }
}

void FSM::onSetup() {
  if (TWCR == 0) {
    Wire.begin();
    telemetry.send("Wire begin");
  }

  telemetry.send("Setting up IMU..");
  imuSensor->setup();
  telemetry.send("IMU setup complete.");

  telemetry.send("Setting up Altimeter..");
  altimeter->setup();
  telemetry.send("Altimeter setup complete.");

  process_event(SETUP_COMPLETE);
}

void FSM::onIDLE() {}

void FSM::onCalibration() {
  telemetry.send("Altimeter: calibrating..");
  altimeter->calibrate();
  telemetry.send("Altimeter: ground level set to " + String(altimeter->getGroundLevel()) + "m");

  telemetry.send("IMU: calibrating..");
  imuSensor->calibrate();
  telemetry.send("IMU: calibrated.");

  process_event(CALIBRATION_COMPLETE);
}

void FSM::onReady() {
  if (altimeter->agl() > LAUNCH_AGL_THRESHOLD or
      imuSensor->accelerationX() / GRAVITY > LAUNCH_ACCELERATION_THRESHOLD) {
    launchTime = millis();
    process_event(LAUNCHED);
  }
}

void FSM::onAscending() {
  if (millis() - launchTime > TIME_TO_APOGEE * 1000) {
    process_event(APOGEE_TIMER_TIMEOUT);
  }
  // TODO: check BMP and IMU
}

void FSM::onApogeeTimeout() {
  telemetry.send("apogee timeout");
  // TODO: trigger and detect ejection charge
}

void FSM::onApogee() {
  telemetry.send("apogee");
  // TODO: trigger and detect ejection charge
}

void FSM::onFTS() {
  telemetry.send("fts");
  // TODO: trigger and detect ejection charge
}

void FSM::onRecovering() {
  telemetry.send("recovering");
  // TODO: celebrate
}


void FSM::runCurrentState() {
  float *values;
  int countValues = 2;
  values = (float*)malloc(countValues * sizeof(float));
  values[0] = freeMemory();
  values[1] = state;
  if (state != SETUP and state != IDLE and state != CALIBRATION) {
    countValues = 12;
    values = (float*)realloc(values, countValues * sizeof(float));
    values[2] = altimeter->getGroundLevel();
    values[3] = altimeter->agl();
    values[4] = altimeter->altitude();
    values[5] = altimeter->pressure();
    values[6] = imuSensor->accelerationX();
    values[7] = imuSensor->accelerationY();
    values[8] = imuSensor->accelerationZ();
    values[9] = imuSensor->gyroX();
    values[10] = imuSensor->gyroY();
    values[11] = imuSensor->gyroZ();
  }
  telemetry.sendValues(values, countValues);
  free(values);

  switch (state) {
    case SETUP:
      onSetup();
      break;
    case IDLE:
      onIDLE();
      break;
    case CALIBRATION:
      onCalibration();
      break;
    case READY:
      onReady();
      break;
    case ASCENDING:
      onAscending();
      break;
    case APOGEE:
      onApogee();
      break;
    case APOGEE_TIMEOUT:
      onApogeeTimeout();
      break;
    case FTS:
      onFTS();
      break;
    case RECOVERING:
      onRecovering();
      break;
  }
}
