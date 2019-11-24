class IMU {
  public:
    virtual float pitch(); // degrees
    virtual float yaw(); // degrees
    virtual float roll(); // degrees

    virtual float accelerationX(); // mg
    virtual float accelerationY(); // mg
    virtual float accelerationZ(); // mg

    virtual float gyroX();
    virtual float gyroY();
    virtual float gyroZ();
};
