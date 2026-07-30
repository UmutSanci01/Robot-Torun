#include "config.h"
#include "lib\IMU\IMU.h"

IMU imu;

void setup()
{
    Serial.begin(115200);

    if (imu.begin())
    {
        Serial.println("IMU OK");
    }
    else
    {
        Serial.println("IMU FAILED");
    }
}

void loop()
{
}