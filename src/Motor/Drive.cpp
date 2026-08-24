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
// {
//     leftTargetRPM_ = left;
//     rightTargetRPM_ = right;

//     leftPid_.setTarget(left);
//     rightPid_.setTarget(right);
// }
{
    leftDesiredRPM_ = left;
    rightDesiredRPM_ = right;
}

float Drive::leftTargetRPM() const
{
    return leftTargetRPM_;
}
float Drive::rightTargetRPM() const
{
    return rightTargetRPM_;
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

    isDriving_ = false;
    isTurning_ = false;
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

    leftPid_.reset();
    rightPid_.reset();

    leftDesiredRPM_ = 0.0f;
    rightDesiredRPM_ = 0.0f;
    leftTargetRPM_ = 0.0f;
    rightTargetRPM_ = 0.0f;
    
    leftPid_.setTarget(0.0f);
    rightPid_.setTarget(0.0f);

    isDriving_ = false;
    isTurning_ = false;
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

    // ASYMETRIC SLEW RATE LIMITER
    if (abs(leftDesiredRPM_) < abs(leftTargetRPM_)) {
        leftTargetRPM_ = leftDesiredRPM_; 
    } else {
        if (leftTargetRPM_ < leftDesiredRPM_) leftTargetRPM_ += accelStep_;
        else if (leftTargetRPM_ > leftDesiredRPM_) leftTargetRPM_ -= accelStep_;
    }

    if (abs(rightDesiredRPM_) < abs(rightTargetRPM_)) {
        rightTargetRPM_ = rightDesiredRPM_;
    } else {
        if (rightTargetRPM_ < rightDesiredRPM_) rightTargetRPM_ += accelStep_;
        else if (rightTargetRPM_ > rightDesiredRPM_) rightTargetRPM_ -= accelStep_;
    }

    leftPid_.setTarget(leftTargetRPM_);
    rightPid_.setTarget(rightTargetRPM_);

    if (leftTargetRPM_ == 0.0f && rightTargetRPM_ == 0.0f)
    {
        return;
    }
    
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

bool Drive::turning() const
{
    return isTurning_;
}
bool Drive::driving() const
{
    return isDriving_;
}

bool Drive::rotateIMU(float _targetDegree, IMU& imu) {
    float currDegree = imu.orientation().yaw;
    float degreeErr = _targetDegree - currDegree;

    while (degreeErr > 180.0f) degreeErr -= 360.0f;
    while (degreeErr < -180.0f) degreeErr += 360.0f;

    float targetRPM = degreeErr * Kp_Degree;

    targetRPM = constrain(targetRPM, -25.0f, 25.0f);

    if (abs(degreeErr) < 0.08f) {
        stop();

        isTurning_ = false;
        return true;
    }

    if (!isTurning_) isTurning_ = true;
    setTargetRPM(targetRPM, -targetRPM);

    return false;
}

void Drive::driveStraightIMU(float baseRPM, float targetDegree, IMU& imu) {
    float currDegree = imu.orientation().yaw;
    float degreeErr = targetDegree - currDegree;

    while (degreeErr > 180.0f) degreeErr -= 360.0f;
    while (degreeErr < -180.0f) degreeErr += 360.0f;

    // Deadband Filter
    if (abs(degreeErr) < 0.5f) {
        degreeErr = 0.0f; 
    }

    // float Kp_Straight = 1.9f;
    float correction = degreeErr * Kp_Straight;

    setTargetRPM(baseRPM + correction, baseRPM - correction);
}

bool Drive::driveDistanceIMU(float targetDistanceCM, float targetDegree, float baseRPM, IMU& imu) {
    // long leftTicks = abs(leftEncoder_.ticks()); 
    // long rightTicks = abs(rightEncoder_.ticks());

    // float averageTicks = (leftTicks + rightTicks) / 2.0f;
    // float currentDistanceCM = (averageTicks / leftEncoder_.ticksPerRevolution()) * MotorConfig::wheelCircumferenceCM;
    float currentDistanceCM = (abs(leftEncoder_.distance()) + abs(rightEncoder_.distance())) / 2.0f * 100.0f;

    if (currentDistanceCM >= targetDistanceCM) {
        stop();
        isDriving_ = false;
        return true;
    }

    float minRPM = 5.0f; 
    float remainingDistance = targetDistanceCM - currentDistanceCM;

    if (!isDriving_) {
        isDriving_ = true;
    }

    float Kp_Distance = 3.5f; 
    float targetRPM = remainingDistance * Kp_Distance;

    targetRPM = constrain(targetRPM, minRPM, baseRPM);

    driveStraightIMU(targetRPM, targetDegree, imu);
    return false;
}