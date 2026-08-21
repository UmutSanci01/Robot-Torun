#include "..\..\lib\IMU\IMU.h"
#include "..\..\config.h"

IMU::IMU()
    :
    initialized_(false)
{
    gyroBias_.clear();

    lastUpdateUs_ = 0;
    deltaTime_ = 0.0f;
}

bool IMU::begin()
{
    Wire.begin(
        Config::I2C_SDA_PIN,
        Config::I2C_SCL_PIN
    );

    Wire.setClock(
        Config::I2C_CLOCK
    );

    if (!sensor_.init())
    {
        initialized_ = false;
        return false;
    }

    sensor_.setAccRange(MPU9250_ACC_RANGE_2G);

    sensor_.setGyrRange(MPU9250_GYRO_RANGE_1000);

    sensor_.enableGyrDLPF();

    sensor_.setGyrDLPF(MPU9250_DLPF_3);

    filter_.begin(IMUConfig::UPDATE_RATE_HZ);

    lastUpdateUs_ = micros();

    initialized_ = true;

    return true;
}

bool IMU::update()
{
    if (!initialized_)
    {
        return false;
    }

    uint32_t now = micros();

    deltaTime_ =
        (now - lastUpdateUs_) *
        1.0e-6f;

    lastUpdateUs_ = now;

    xyzFloat acc = sensor_.getGValues();
    xyzFloat gyr = sensor_.getGyrValues();

    accel_.x = acc.x;
    accel_.y = acc.y;
    accel_.z = acc.z;

    gyro_.x = gyr.x - gyroBias_.x;
    gyro_.y = gyr.y - gyroBias_.y;
    gyro_.z = gyr.z - gyroBias_.z;

    gyro_.x = (gyro_.x > -0.50f && gyro_.x < 0.50f) ? 0.0f : gyro_.x;
    gyro_.y = (gyro_.y > -0.50f && gyro_.y < 0.50f) ? 0.0f : gyro_.y;
    gyro_.z = (gyro_.z > -0.50f && gyro_.z < 0.50f) ? 0.0f : gyro_.z;

    filter_.updateIMU(
        gyro_.x,
        gyro_.y,
        gyro_.z,
        accel_.x,
        accel_.y,
        accel_.z,
        deltaTime_
    );

    orientation_.roll  = filter_.getRoll();
    orientation_.pitch = filter_.getPitch();
    orientation_.yaw   = filter_.getYaw();

    return true;
}

bool IMU::calibrate()
{
    if (!initialized_)
    {
        return false;
    }

    gyroBias_.clear();

    for(uint16_t i = 0; i < IMUConfig::CALIBRATION_SAMPLES; i++)
    {
        xyzFloat gyr = sensor_.getGyrValues();

        gyroBias_.x += gyr.x;
        gyroBias_.y += gyr.y;
        gyroBias_.z += gyr.z;

        delay(2);
    }

    gyroBias_.x /= IMUConfig::CALIBRATION_SAMPLES;
    gyroBias_.y /= IMUConfig::CALIBRATION_SAMPLES;
    gyroBias_.z /= IMUConfig::CALIBRATION_SAMPLES;

    return true;
}

bool IMU::healthy() const
{
    return initialized_;
}

float IMU::deltaTime() const
{
    return deltaTime_;
}

const Vector3f& IMU::accel() const
{
    return accel_;
}

const Vector3f& IMU::gyro() const
{
    return gyro_;
}

const Euler& IMU::orientation() const
{
    return orientation_;
}

GyroBias IMU::getGyroBias() const
{
    return
    {
        gyroBias_.x,
        gyroBias_.y,
        gyroBias_.z
    };
}

void IMU::setGyroBias(const GyroBias& bias)
{
    gyroBias_.x = bias.x;
    gyroBias_.y = bias.y;
    gyroBias_.z = bias.z;
}