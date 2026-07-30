#pragma once

#include <Wire.h>
#include <MPU9250_WE.h>
#include <Arduino.h>
#include <Adafruit_AHRS.h>

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

    bool begin();

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

    MPU6500_WE sensor_;

    bool initialized_;

    Vector3f accel_;

    Vector3f gyro_;

    Vector3f gyroBias_;

    Euler orientation_;

    Adafruit_Mahony filter_;

    uint32_t lastUpdateUs_;

    float deltaTime_;
};