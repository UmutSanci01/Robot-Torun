#pragma once

#include <Arduino.h>
#include "Motor.h"
#include "..\Encoder\Encoder.h"

class Drive
{
public:

    Drive(
        Motor& leftMotor,
        Motor& rightMotor,
        Encoder& leftEncoder,
        Encoder& rightEncoder
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

    void setPower(int8_t power);

    int8_t power() const;

    float leftDistance() const;

float rightDistance() const;

void resetDistance();

private:

    Motor& leftMotor_;
    Motor& rightMotor_;

    Encoder& leftEncoder_;
    Encoder& rightEncoder_;

    int8_t power_;
};