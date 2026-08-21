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

bool Drive::turning() const
{
    return isTurning_;
}
bool Drive::driving() const
{
    return isDriving_;
}

void Drive::rotateIMU(float _targetDegree, IMU& imu) {
    float currDegree = imu.orientation().yaw;
    float degreeErr = _targetDegree - currDegree;

    while (degreeErr > 180.0f) degreeErr -= 360.0f;
    while (degreeErr < -180.0f) degreeErr += 360.0f;

    float targetRPM = degreeErr * Kp_Degree;

    targetRPM = constrain(targetRPM, -75.0f, 75.0f);

    if (abs(degreeErr) < 0.100f) {
        stop();
        disable();

        isTurning_ = false;
    } else {
        if (!isTurning_) isTurning_ = true;
        setTargetRPM(targetRPM, -targetRPM);
    }
}

void Drive::driveStraightIMU(float baseRPM, float targetDegree, IMU& imu) {
    float currDegree = imu.orientation().yaw;
    float degreeErr = targetDegree - currDegree;

    while (degreeErr > 180.0f) degreeErr -= 360.0f;
    while (degreeErr < -180.0f) degreeErr += 360.0f;

    float Kp_Straight = 1.0f;
    float correction = degreeErr * Kp_Straight;

    setTargetRPM(baseRPM + correction, baseRPM - correction);
}

bool Drive::driveDistanceIMU(float targetDistanceCM, float targetDegree, float baseRPM, IMU& imu) {
    long leftTicks = abs(leftEncoder_.ticks()); 
    long rightTicks = abs(rightEncoder_.ticks());

    float averageTicks = (leftTicks + rightTicks) / 2.0f;
    float currentDistanceCM = (averageTicks / leftEncoder_.ticksPerRevolution()) * MotorConfig::wheelCircumferenceCM;

    if (currentDistanceCM >= targetDistanceCM) {
        stop();
        disable();
        isDriving_ = false;
        return true;
    }

    static float currentProfileRPM = 15.0f; 
    float minRPM = 15.0f; 
    float remainingDistance = targetDistanceCM - currentDistanceCM;

    if (!isDriving_) {
        currentProfileRPM = minRPM;
        isDriving_ = true;
    }

    float Kp_Distance = 3.5f; 
    float maxAllowedRPM = remainingDistance * Kp_Distance;

    maxAllowedRPM = constrain(maxAllowedRPM, minRPM, baseRPM);

    float accelerationStep = 0.5f;

    if (currentProfileRPM < maxAllowedRPM) {
        currentProfileRPM += accelerationStep; // Kademeli hızlan
        if (currentProfileRPM > maxAllowedRPM) {
            currentProfileRPM = maxAllowedRPM;
        }
    } else {
        currentProfileRPM = maxAllowedRPM; 
    }
    // ------------------------------------

    driveStraightIMU(currentProfileRPM, targetDegree, imu);
    return false;
}

// bool Drive::driveDistanceIMU(float targetDistanceCM, float targetDegree, float baseRPM, IMU& imu) {
//     long leftTicks = abs(leftEncoder_.ticks()); 
//     long rightTicks = abs(rightEncoder_.ticks());

//     float averageTicks = (leftTicks + rightTicks) / 2.0f;

//     // float currentDistanceCM = (averageTicks / MotorConfig::ticksPerRev) * MotorConfig::wheelCircumferenceCM;
//     float currentDistanceCM = (averageTicks / leftEncoder_.ticksPerRevolution()) * MotorConfig::wheelCircumferenceCM;

//     if (currentDistanceCM >= targetDistanceCM) {
//         stop();
//         disable();

//         isDriving_ = false;
//         return true;
//     }

//     float currentTargetRPM = baseRPM;
//     float rampUpDistance = 10.0f; // Soft acceleration distance(cm)
//     float rampDownDistance = 15.0f; // Soft stop distance(cm)
//     float minRPM = 5.0f;

//     float remainingDistance = targetDistanceCM - currentDistanceCM;

//     if (currentDistanceCM < rampUpDistance) {
//         currentTargetRPM = minRPM + ((baseRPM - minRPM) * (currentDistanceCM / rampUpDistance));
//     }
//     else if (remainingDistance < rampDownDistance) {
//         currentTargetRPM = minRPM + ((baseRPM - minRPM) * (remainingDistance / rampDownDistance));
//     }

//     if (!isDriving_) isDriving_ = true;
//     driveStraightIMU(currentTargetRPM, targetDegree, imu);
//     return false;
// }