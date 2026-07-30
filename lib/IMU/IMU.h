#pragma once

#include <Wire.h>
#include <MPU9250_WE.h>
#include <Arduino.h>

#include "Vector3.h"
#include "Euler.h"

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
    

private:

    MPU6500_WE sensor_;

    bool initialized_;

    Vector3f accel_;

    Vector3f gyro_;

    Vector3f gyroBias_;

    Euler orientation_;

    uint32_t lastUpdateUs_;

    float deltaTime_;
};