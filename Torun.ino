#include "lib/IMU/IMU.h"

IMU imu;

void setup()
{
    Serial.begin(115200);

    imu.begin();
}

void loop()
{
    imu.update();

    const Euler& e = imu.orientation();

    Serial.printf(
        "R:%7.2f  P:%7.2f  Y:%7.2f\n",
        e.roll,
        e.pitch,
        e.yaw
    );

    delay(50);
}