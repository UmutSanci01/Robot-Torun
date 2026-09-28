#pragma once

#include <Arduino.h>

/*----------------------------------------------------------
    Hardware Configuration
----------------------------------------------------------*/

#define TARGET_ROBOT 1

namespace Config
{
    constexpr uint8_t I2C_SDA_PIN = 21;
    constexpr uint8_t I2C_SCL_PIN = 22;

    constexpr uint32_t I2C_CLOCK = 400000UL;

    constexpr uint8_t MPU6500_ADDRESS = 0x68;
    constexpr uint8_t BMI160_ADDRESS = 0x69;

    constexpr float kp = 2.f, ki = 14.f, kd = 0.002f;
}

/*----------------------------------------------------------
    IMU Configuration
----------------------------------------------------------*/

namespace IMUConfig
{
    constexpr float UPDATE_RATE_HZ = 200.0f;
    constexpr float UPDATE_PERIOD = 1.0f / UPDATE_RATE_HZ;

    constexpr uint16_t CALIBRATION_SAMPLES = 300;
}

namespace EncoderConfig
{
    #if TARGET_ROBOT == 1
        static constexpr float GEAR_RATIO = 211.607f;
    #elif TARGET_ROBOT == 2
        static constexpr float GEAR_RATIO = 389.786f;
    #endif
    static constexpr float PPR = 7.0f;
    static constexpr float QUAD = 4.0f;
    static constexpr float DIAMETER = 0.04438f;

    static constexpr float TICKS_PER_REV = GEAR_RATIO * PPR * QUAD;
}

namespace MotorConfig
{
    constexpr int8_t MAX_POWER = 100;
    constexpr uint32_t PWM_FREQUENCY = 20000;
    constexpr uint8_t PWM_RESOLUTION = 8;

    constexpr uint8_t LEFT_IN1_CHANNEL  = 0;
    constexpr uint8_t LEFT_IN2_CHANNEL  = 1;

    constexpr uint8_t RIGHT_IN1_CHANNEL = 2;
    constexpr uint8_t RIGHT_IN2_CHANNEL = 3;
}