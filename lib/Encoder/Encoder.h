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

    void reset();

    int32_t ticks() const;

    float revolutions() const;

    float distance() const;

    void setTicksPerRevolution(float ticks);

    void setWheelDiameter(float diameter);

    float ticksPerRevolution() const;

    float wheelDiameter() const;

private:
    uint32_t previousUpdateMs_;
    float ticksPerRevolution_ = 1.0f;
    float wheelDiameter_ = 1.0f;
    
    uint8_t pinA_;
    uint8_t pinB_;

    bool initialized_;

    int32_t previousPosition_;

    int32_t delta_;

    float rpm_;

    float velocity_;

    float distance_;

    int32_t count_;

    static Encoder* instance0_;
    static Encoder* instance1_;

    uint8_t instanceIndex_;

    volatile int32_t position_;

    volatile uint8_t previousState_;

    static void IRAM_ATTR isr0();
    static void IRAM_ATTR isr1();

    void IRAM_ATTR handleInterrupt();

    static constexpr float GEAR_RATIO = 210.0f;
    static constexpr float PPR = 7.0f;
    static constexpr float QUAD = 4.0f;

    static constexpr float TICKS_PER_REV =
        GEAR_RATIO * PPR * QUAD;

    static constexpr float WHEEL_DIAMETER = 0.042f;
};