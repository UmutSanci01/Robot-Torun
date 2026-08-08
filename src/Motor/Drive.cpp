#include "..\..\lib\Motor\Drive.h"

Drive::Drive(
    Motor& leftMotor,
    Motor& rightMotor,
    Encoder& leftEncoder,
    Encoder& rightEncoder
)
:
leftMotor_(leftMotor),
rightMotor_(rightMotor),
leftEncoder_(leftEncoder),
rightEncoder_(rightEncoder),
power_(0)
{
}

bool Drive::begin()
{
    leftPid_.setOutputLimits(
        -MotorConfig::MAX_POWER,
        MotorConfig::MAX_POWER);

    rightPid_.setOutputLimits(
        -MotorConfig::MAX_POWER,
        MotorConfig::MAX_POWER);

    // leftPid_.setTunings(1.f, 0, 0);
    // rightPid_.setTunings(1.f, 0, 0);

    previousPidMs_ = millis();

    return true;
}

void Drive::setTargetRPM(
    float left,
    float right)
{
    leftTargetRPM_ = left;
    rightTargetRPM_ = right;

    leftPid_.setTarget(left);
    rightPid_.setTarget(right);
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

void Drive::setPIDTunings(
    float kp,
    float ki,
    float kd)
{
    leftPid_.setTunings(kp, ki, kd);
    rightPid_.setTunings(kp, ki, kd);

    leftPid_.reset();
    rightPid_.reset();
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

float Drive::leftPIDOutput() const
{
    return leftPid_.output();
}

float Drive::rightPIDOutput() const
{
    return rightPid_.output();
}

void Drive::setPower(int8_t power)
{
    power_ = constrain(power,-MotorConfig::MAX_POWER,MotorConfig::MAX_POWER);

    setPower(power_,power_);
}

void Drive::update()
{
    uint32_t now = millis();

    float dt =
        (now - previousPidMs_) /
        1000.0f;

    if(dt < 0.02f)
        return;

    previousPidMs_ = now;

    float left =
        leftPid_.update(
            leftEncoder_.rpm(),
            dt);

    float right =
        rightPid_.update(
            rightEncoder_.rpm(),
            dt);

    leftMotor_.setPower(
        (int8_t)left);

    rightMotor_.setPower(
        (int8_t)right);
}

int8_t Drive::power() const
{
    return power_;
}

float Drive::leftDistance() const
{
    return leftEncoder_.distance();
}

float Drive::rightDistance() const
{
    return rightEncoder_.distance();
}

void Drive::resetDistance()
{
    leftEncoder_.reset();
    rightEncoder_.reset();
}

void Drive::forward(int8_t power)
{
    setPower(power);
}

void Drive::backward(int8_t power)
{
    setPower(-power);
}

void Drive::rotateLeft(int8_t power)
{
    setPower(-power, power);
}

void Drive::rotateRight(int8_t power)
{
    setPower(power, -power);
}

void Drive::arc(int8_t leftPower, int8_t rightPower)
{
    setPower(leftPower, rightPower);
}

void Drive::brake()
{
    stop();
}

Encoder& Drive::leftEncoder()
{
    return leftEncoder_;
}

Encoder& Drive::rightEncoder()
{
    return rightEncoder_;
}