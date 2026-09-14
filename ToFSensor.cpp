#include "ToFSensor.h"

ToFSensor::ToFSensor(uint8_t i2c_address) {
    address = i2c_address;
    lastReadTime = 0;
    lastDistance = 0;
    isInitialized = false;
}

bool ToFSensor::begin() {
    Wire.beginTransmission(address);
    Wire.write(0xC0);
    if (Wire.endTransmission(false) == 0) {
        Wire.requestFrom((uint8_t)address, (uint8_t)1);
        if (Wire.available()) {
            uint8_t id = Wire.read();
            if (id == 0xEE) {
                // Sürekli ölçüm modunu başlat
                Wire.beginTransmission(address);
                Wire.write(0x00);
                Wire.write(0x02);
                Wire.endTransmission();
                isInitialized = true;
                return true;
            }
        }
    }
    return false;
}

void ToFSensor::update() {
    if (!isInitialized) return;

    if (millis() - lastReadTime >= READ_INTERVAL) {
        lastReadTime = millis();

        Wire.beginTransmission(address);
        Wire.write(0x1E);
        Wire.endTransmission(false);

        Wire.requestFrom((uint8_t)address, (uint8_t)2);
        if (Wire.available() >= 2) {
            uint16_t raw_dist = (Wire.read() << 8) | Wire.read();

            const uint16_t OFFSET = 50; 
            
            if (raw_dist > OFFSET) {
                lastDistance = raw_dist - OFFSET;
            }
        }
    }
}

uint16_t ToFSensor::getDistance() {
    return lastDistance;
}

bool ToFSensor::isReady() {
    return isInitialized;
}