#include "..\..\lib\IMU\Mahony.h"

#include <math.h>

Mahony::Mahony()
{
    reset();
}

void Mahony::reset()
{
    q0_ = 1.0f;
    q1_ = 0.0f;
    q2_ = 0.0f;
    q3_ = 0.0f;

    integralX_ = 0.0f;
    integralY_ = 0.0f;
    integralZ_ = 0.0f;

    updateEuler();
}

void Mahony::update(
    float gx,
    float gy,
    float gz,
    float ax,
    float ay,
    float az,
    float dt)
{
    (void)gx;
    (void)gy;
    (void)gz;
    (void)ax;
    (void)ay;
    (void)az;
    (void)dt;

    updateEuler();
}

Euler Mahony::euler() const
{
    return euler_;
}

void Mahony::updateEuler()
{
    euler_.roll =
        atan2f(
            2.0f * (q0_ * q1_ + q2_ * q3_),
            1.0f - 2.0f * (q1_ * q1_ + q2_ * q2_)
        ) * RAD_TO_DEG;

    float sinPitch =
        2.0f * (q0_ * q2_ - q3_ * q1_);

    if (sinPitch > 1.0f)
    {
        sinPitch = 1.0f;
    }
    else if (sinPitch < -1.0f)
    {
        sinPitch = -1.0f;
    }

    euler_.pitch =
        asinf(sinPitch) * RAD_TO_DEG;

    euler_.yaw =
        atan2f(
            2.0f * (q0_ * q3_ + q1_ * q2_),
            1.0f - 2.0f * (q2_ * q2_ + q3_ * q3_)
        ) * RAD_TO_DEG;
}