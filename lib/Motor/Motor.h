#pragma once

#include <Arduino.h>

class Motor
{
public:

    Motor(
        uint8_t in1,
        uint8_t in2,
        uint8_t enablePin
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
};