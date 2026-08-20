#pragma once

namespace MotorConfig
{
    constexpr int8_t MAX_POWER = 100;
    constexpr uint32_t PWM_FREQUENCY = 20000;
    constexpr uint8_t PWM_RESOLUTION = 8;

    constexpr uint8_t LEFT_IN1_CHANNEL  = 0;
    constexpr uint8_t LEFT_IN2_CHANNEL  = 1;

    constexpr uint8_t RIGHT_IN1_CHANNEL = 2;
    constexpr uint8_t RIGHT_IN2_CHANNEL = 3;

    constexpr float ticksPerRev = 5925.0f; 
    constexpr float wheelCircumferenceCM = 4.4f * PI; // 13.823 cm
}