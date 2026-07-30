#include "..\..\lib\IMU\IMU.h"

IMU::IMU()
    :
    initialized_(false)
{
}

bool IMU::begin()
{
    initialized_ = true;

    return initialized_;
}

bool IMU::update()
{
    if (!initialized_)
    {
        return false;
    }

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