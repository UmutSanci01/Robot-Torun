#ifndef QMC5883_H
#define QMC5883_H

#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include "Config.h"

class QMC5883 {
private:
    const uint8_t ADDR = 0x2C;
    TwoWire* _wire;
    Preferences& _pref;

    // Hard Iron Offsets
    float hardOffsetX = 0.0f;
    float hardOffsetY = 0.0f;
    float hardOffsetZ = 0.0f;

    // Soft Iron Scales
    float softScaleX = 1.0f;
    float softScaleY = 1.0f;
    float softScaleZ = 1.0f;

    float mX, mY, mZ; 
    
    bool isCalibrating;
    float minX, maxX, minY, maxY, minZ, maxZ;

    void saveCalibration(float hx, float hy, float hz, float sx, float sy, float sz);
    void loadCalibration();
    
public:
    QMC5883(Preferences& pref);
    void begin(TwoWire* wire = &Wire);
    void update();
    
    void startCalibration();
    void finishCalibration();

    float getX() { return mX; }
    float getY() { return mY; }
    float getZ() { return mZ; }
    
    void setCalibration(float hx, float hy, float hz, float sx, float sy, float sz);
};

#endif