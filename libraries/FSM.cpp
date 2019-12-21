#include "FSM.h"

Telemetry telemetry = Telemetry::getInstance();

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
  float values[] = {
    altimeter->getGroundLevel(),
    altimeter->agl(),
    altimeter->altitude(),
    altimeter->pressure(),
    imuSensor->accelerationX(),
    imuSensor->accelerationY(),
    imuSensor->accelerationZ(),
    imuSensor->gyroX(),
    imuSensor->gyroY(),
    imuSensor->gyroZ()
  };
  telemetry.sendValues(values, 10);
  // TODO: check BMP and IMU for acceleration and altitude change
}

void FSM::onAscending() {
  telemetry.send("ascending");
  // TODO: check BMP and IMU
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
  // TODO: Send raw data in telemetry
  telemetry.send("State: " + String(state));
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
    case FTS:
      onFTS();
      break;
    case RECOVERING:
      onRecovering();
      break;
  }
}
