#include "lib/Motor/Motor.h"

Motor leftMotor(26, 25, 14);
Motor rightMotor(32, 33, 14);

void setup()
{
    leftMotor.begin();
    rightMotor.begin();

    leftMotor.enable();
    rightMotor.enable();

    digitalWrite(14, LOW);
}

void loop()
{
    leftMotor.setPower(100);
    rightMotor.setPower(100);

    delay(2000);

    leftMotor.stop();
    rightMotor.stop();

    delay(1000);

    leftMotor.setPower(-100);
    rightMotor.setPower(-100);

    delay(2000);

    leftMotor.stop();
    rightMotor.stop();

    delay(3000);
}