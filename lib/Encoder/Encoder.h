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

    int32_t previousPosition_;

    int32_t delta_;

    float rpm_;

    float velocity_;

    float distance_;

    static Encoder* instance0_;
    static Encoder* instance1_;

    uint8_t instanceIndex_;

    volatile int32_t position_;

    volatile uint8_t previousState_;

    static void IRAM_ATTR isr0();
    static void IRAM_ATTR isr1();

    void IRAM_ATTR handleInterrupt();
};