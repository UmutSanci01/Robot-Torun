#ifndef TOFSENSOR_H
#define TOFSENSOR_H

#include <Arduino.h>
#include <Wire.h>

class ToFSensor {
private:
    uint8_t address;
    uint32_t lastReadTime;
    uint16_t lastDistance;
    bool isInitialized;
    const uint16_t READ_INTERVAL = 50; // 50 ms'de bir okuma (Non-blocking)

public:
    ToFSensor(uint8_t i2c_address = 0x29);
    bool begin();
    void update();
    uint16_t getDistance();
    bool isReady();
};

#endif