#pragma once

#include <Arduino.h>

class Motor
{
public:

    Motor(
        uint8_t in1,
        uint8_t in2,
        uint8_t enablePin,
        uint8_t pwmChannel
    );

    bool begin();

    void enable();

    void disable();

    bool enabled() const;

    void setPower(int8_t power);

    void stop();

private:

    uint8_t in1_;
    uint8_t in2_;
    uint8_t enablePin_;

    bool enabled_;

    int8_t power_;

    uint8_t pwmChannel_;
    uint32_t pwmFrequency_;
    uint8_t pwmResolution_;
};