#include "ToFSensor.h"

ToFSensor::ToFSensor(uint8_t i2c_address) {
    address = i2c_address;
    lastReadTime = 0;
    lastDistance = 0;
    isInitialized = false;
    _wire = &Wire;
}

bool ToFSensor::begin(TwoWire* wire) {
    _wire->beginTransmission(address);
    _wire->write(0xC0);
    if (_wire->endTransmission(false) == 0) {
        _wire->requestFrom((uint8_t)address, (uint8_t)1);
        if (_wire->available()) {
            uint8_t id = _wire->read();
            if (id == 0xEE) {
                // Sürekli ölçüm modunu başlat
                _wire->beginTransmission(address);
                _wire->write(0x00);
                _wire->write(0x02);
                _wire->endTransmission();
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

        _wire->beginTransmission(address);
        _wire->write(0x1E);
        _wire->endTransmission(false);

        _wire->requestFrom((uint8_t)address, (uint8_t)2);
        if (_wire->available() >= 2) {
            uint16_t raw_dist = (_wire->read() << 8) | _wire->read();

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