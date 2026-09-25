#pragma once

#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_AHRS.h>

#include "../../Config.h"

#if TARGET_ROBOT == 1
    #include <MPU9250_WE.h>
#elif TARGET_ROBOT == 2
    #include "../../DFRobot_BMI160.h"
#endif

#include "Vector3.h"
#include "Euler.h"

struct GyroBias
{
    float x;
    float y;
    float z;
};

class IMU
{
public:

    IMU();

    bool begin(TwoWire* wire = &Wire);

    bool update();

    bool calibrate();

    bool healthy() const;

    float deltaTime() const;

    const Vector3f& accel() const;

    const Vector3f& gyro() const;

    const Euler& orientation() const;

    GyroBias getGyroBias() const;

    void setGyroBias(const GyroBias& bias);
    

private:
    TwoWire* _wire;
    
    #if TARGET_ROBOT == 1
        MPU6500_WE sensor_;
    #elif TARGET_ROBOT == 2
        DFRobot_BMI160 sensor_;
    #endif

    bool initialized_;

    Vector3f accel_;

    Vector3f gyro_;

    Vector3f gyroBias_;

    Euler orientation_;

    Adafruit_Mahony filter_;

    uint32_t lastUpdateUs_;

    float deltaTime_;
};