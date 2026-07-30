#pragma once

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

    const Vector3f& accel() const;

    const Vector3f& gyro() const;

    const Euler& orientation() const;

private:

    bool initialized_;

    Vector3f accel_;

    Vector3f gyro_;

    Euler orientation_;
};