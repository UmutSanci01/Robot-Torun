#pragma once

#include <Arduino.h>

/*----------------------------------------------------------
    Hardware Configuration
----------------------------------------------------------*/

namespace Config
{
    constexpr uint8_t I2C_SDA_PIN = 21;
    constexpr uint8_t I2C_SCL_PIN = 22;

    constexpr uint32_t I2C_CLOCK = 400000UL;

    constexpr uint8_t MPU6500_ADDRESS = 0x68;
}

/*----------------------------------------------------------
    IMU Configuration
----------------------------------------------------------*/

namespace IMUConfig
{
    constexpr float UPDATE_RATE_HZ = 200.0f;
    constexpr float UPDATE_PERIOD = 1.0f / UPDATE_RATE_HZ;

    constexpr uint16_t CALIBRATION_SAMPLES = 2000;
}