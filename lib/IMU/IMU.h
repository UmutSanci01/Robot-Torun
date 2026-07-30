class IMU
{
public:

    bool begin();

    bool calibrate();

    bool update();

    float roll() const;
    float pitch() const;
    float yaw() const;

    const Euler& orientation() const;

    const Vector3f& accel() const;

    const Vector3f& gyro() const;

    bool healthy() const;

private:

    ...
};