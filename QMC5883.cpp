#include "QMC5883.h"

QMC5883::QMC5883() {
    mX = 0; mY = 0; mZ = 0;
    isCalibrating = false;
}

void QMC5883::begin() {
    Wire.beginTransmission(ADDR);
    Wire.write(0x0A);
    Wire.write(0xCF);
    Wire.endTransmission();
    Serial.println("QMC5883P Class: Dogru register ile baslatildi.");
}

void QMC5883::update() {
    Wire.beginTransmission(ADDR);
    Wire.write(0x09); 
    Wire.endTransmission(false);
    Wire.requestFrom(ADDR, (uint8_t)1);
    
    if (!Wire.available()) return;
    uint8_t status = Wire.read();

    if (status & 0x01) {
        Wire.beginTransmission(ADDR);
        Wire.write(0x01);
        Wire.endTransmission(false);
        Wire.requestFrom(ADDR, (uint8_t)6);
        
        if (Wire.available() >= 6) {
            uint8_t x_lsb = Wire.read();
            uint8_t x_msb = Wire.read();
            uint8_t y_lsb = Wire.read();
            uint8_t y_msb = Wire.read();
            uint8_t z_lsb = Wire.read();
            uint8_t z_msb = Wire.read();

            int16_t raw_x = (int16_t)((x_msb << 8) | x_lsb);
            int16_t raw_y = (int16_t)((y_msb << 8) | y_lsb);
            int16_t raw_z = (int16_t)((z_msb << 8) | z_lsb);

            float mag_x = (float)raw_y;
            float mag_y = (float)raw_x;
            float mag_z = (float)raw_z;

            if (isCalibrating) {
                if (mag_x < minX) minX = mag_x;
                if (mag_x > maxX) maxX = mag_x;
                if (mag_y < minY) minY = mag_y;
                if (mag_y > maxY) maxY = mag_y;
                if (mag_z < minZ) minZ = mag_z;
                if (mag_z > maxZ) maxZ = mag_z;
            } 
            else {
                float cal_x = mag_x - hardOffsetX;
                float cal_y = mag_y - hardOffsetY;
                float cal_z = mag_z - hardOffsetZ;

                cal_x *= softScaleX;
                cal_y *= softScaleY;
                cal_z *= softScaleZ;

                if (mX == 0 && mY == 0) {
                    mX = cal_x; mY = cal_y; mZ = cal_z;
                } else {
                    mX = (cal_x * 0.15f) + (mX * 0.85f);
                    mY = (cal_y * 0.15f) + (mY * 0.85f);
                    mZ = (cal_z * 0.15f) + (mZ * 0.85f);
                }
            }
        }
    }
}

void QMC5883::startCalibration() {
    minX = minY = minZ = 32000.0f;
    maxX = maxY = maxZ = -32000.0f;
    isCalibrating = true;
    Serial.println("Magnetometer calibration has been started...");
}

void QMC5883::finishCalibration() {
    isCalibrating = false;
    
    hardOffsetX = (maxX + minX) / 2.0f;
    hardOffsetY = (maxY + minY) / 2.0f;
    hardOffsetZ = (maxZ + minZ) / 2.0f;
    
    float chordX = (maxX - minX) / 2.0f;
    float chordY = (maxY - minY) / 2.0f;
    float chordZ = (maxZ - minZ) / 2.0f;
    
    float avg_chord = (chordX + chordY + chordZ) / 3.0f;
    
    softScaleX = avg_chord / chordX;
    softScaleY = avg_chord / chordY;
    softScaleZ = avg_chord / chordZ;
    
    // Serial.println("--- CALIBRATION COMPLETED ---");
    // Serial.printf("Hard Iron Offsets -> X: %.2f | Y: %.2f | Z: %.2f\n", hardOffsetX, hardOffsetY, hardOffsetZ);
    // Serial.printf("Soft Iron Scales  -> X: %.2f | Y: %.2f | Z: %.2f\n", softScaleX, softScaleY, softScaleZ);
}

void QMC5883::setCalibration(float hx, float hy, float hz, float sx, float sy, float sz) {
    hardOffsetX = hx; hardOffsetY = hy; hardOffsetZ = hz;
    softScaleX = sx; softScaleY = sy; softScaleZ = sz;
}