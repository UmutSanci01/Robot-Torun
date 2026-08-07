#include "PID.h"
#include <Arduino.h>

PID::PID(
    float kp,
    float ki,
    float kd)
:
kp_(kp),
ki_(ki),
kd_(kd),
target_(0.0f),
integral_(0.0f),
previousError_(0.0f),
output_(0.0f),
minOutput_(-100.0f),
maxOutput_(100.0f)
{
}

void PID::setTunings(float kp,float ki,float kd)
{
    kp_=kp;
    ki_=ki;
    kd_=kd;
}

void PID::setOutputLimits(float minimum,float maximum)
{
    minOutput_=minimum;
    maxOutput_=maximum;
}

void PID::setTarget(float target)
{
    target_=target;
}

void PID::reset()
{
    integral_=0.0f;
    previousError_=0.0f;
    output_=0.0f;
}

float PID::update(float input,float dt)
{
    if(dt<=0.0f)
        return output_;

    float error=target_-input;

    integral_+=error*dt;

    float derivative=
        (error-previousError_)/dt;

    output_=
        kp_*error+
        ki_*integral_+
        kd_*derivative;

    output_=constrain(
        output_,
        minOutput_,
        maxOutput_);

    previousError_=error;

    return output_;
}

float PID::output() const
{
    return output_;
}