#include "config.h"
#include "lib\IMU\IMU.h"

IMU imu;

void setup()
{
    Serial.begin(115200);

    if (!imu.begin())
    {
        Serial.println("IMU ERROR");

        while (true)
        {
        }
    }

    Serial.println("IMU READY");

    Serial.println("Calibrating IMU...");

    imu.calibrate();

    Serial.println("Calibration OK");
}

void loop()
{
    imu.update();

    const Vector3f& a = imu.accel();
    const Vector3f& g = imu.gyro();

    Serial.printf(
        "ACC : %7.3f %7.3f %7.3f | "
        "GYR : %7.3f %7.3f %7.3f\n",
        a.x,
        a.y,
        a.z,
        g.x,
        g.y,
        g.z
    );

    Serial.printf(
        "dt = %.6f\n",
        imu.deltaTime()
    );

    delay(50);
}