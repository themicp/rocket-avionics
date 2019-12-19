class FSM {
  public:
    enum STATE {SETUP, CALIBRATION, READY, ASCENDING, APOGEE, FTS, RECOVERING};
    enum EVENT {INIT_CALIBRATION, CALIBRATION_COMPLETE, LAUNCHED, APOGEE_DETECTED, TRIGGER_FTS, CHUTE_EJECTED};

    void process_event(FSM::EVENT event);
    void runCurrentState();

  private:
    FSM::STATE state = SETUP;

    void onSetup();
    void onCalibration();
    void onReady();
    void onAscending();
    void onApogee();
    void onFTS();
    void onRecovering();
};
