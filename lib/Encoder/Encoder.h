#pragma once

#include <Arduino.h>

class Encoder
{
public:

    Encoder(
        uint8_t pinA,
        uint8_t pinB
    );

    bool begin();

    bool update();

    bool healthy() const;

    int32_t position() const;

    int32_t delta() const;

    float rpm() const;

    float velocity() const;

    float distance() const;

    void reset();

private:

    uint8_t pinA_;
    uint8_t pinB_;

    bool initialized_;

    int32_t position_;

    int32_t previousPosition_;

    int32_t delta_;

    float rpm_;

    float velocity_;

    float distance_;
};