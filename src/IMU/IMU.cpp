#include "..\..\lib\IMU\IMU.h"
#include "..\..\config.h"

IMU::IMU()
    :
    initialized_(false)
{
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

    sensor_.setGyrRange(MPU9250_GYRO_RANGE_250);

    sensor_.enableGyrDLPF();

    sensor_.setGyrDLPF(MPU9250_DLPF_3);

    initialized_ = true;

    return true;
}

bool IMU::update()
{
    if (!initialized_)
    {
        return false;
    }

    xyzFloat acc = sensor_.getGValues();
    xyzFloat gyr = sensor_.getGyrValues();

    accel_.x = acc.x;
    accel_.y = acc.y;
    accel_.z = acc.z;

    gyro_.x = gyr.x;
    gyro_.y = gyr.y;
    gyro_.z = gyr.z;

    return true;
}

bool IMU::calibrate()
{
    if (!initialized_)
    {
        return false;
    }

    return true;
}

bool IMU::healthy() const
{
    return initialized_;
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