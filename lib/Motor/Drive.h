#pragma once

#include <Arduino.h>
#include "Motor.h"

class Drive
{
public:

    Drive(
        Motor& leftMotor,
        Motor& rightMotor
    );

    bool begin();

    void enable();

    void disable();

    bool enabled() const;

    void stop();

    void setPower(
        int8_t left,
        int8_t right
    );

private:

    Motor& leftMotor_;
    Motor& rightMotor_;
};