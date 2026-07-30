#pragma once

#include <Arduino.h>

#include "Euler.h"

class Mahony
{
public:

    Mahony();

    void reset();

    void update(
        float gx,
        float gy,
        float gz,
        float ax,
        float ay,
        float az,
        float dt);

    Euler euler() const;

private:

    float q0_;
    float q1_;
    float q2_;
    float q3_;

    float integralX_;
    float integralY_;
    float integralZ_;

    static constexpr float kKp = 2.0f;
    static constexpr float kKi = 0.02f;

    Euler euler_;

    void updateEuler();
};