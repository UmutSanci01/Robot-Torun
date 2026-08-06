#include "..\..\lib\Motor\Drive.h"

Drive::Drive(
    Motor& leftMotor,
    Motor& rightMotor
)
:
leftMotor_(leftMotor),
rightMotor_(rightMotor),
power_(0)
{
}

bool Drive::begin()
{
    return true;
}

void Drive::enable()
{
    leftMotor_.enable();
    rightMotor_.enable();
}

void Drive::disable()
{
    leftMotor_.disable();
    rightMotor_.disable();
}

bool Drive::enabled() const
{
    return leftMotor_.isEnabled();
}

void Drive::stop()
{
    leftMotor_.stop();
    rightMotor_.stop();
}

void Drive::setPower(
    int8_t left,
    int8_t right
)
{
    leftMotor_.setPower(left);
    rightMotor_.setPower(right);
}

void Drive::setPower(int8_t power)
{
    power_ = constrain(power,-100,100);

    setPower(power_,power_);
}

int8_t Drive::power() const
{
    return power_;
}

