#include "lib/IMU/Mahony.h"

Mahony filter;

void setup()
{
    Serial.begin(115200);

    Euler e = filter.euler();

    Serial.printf(
        "%.2f %.2f %.2f\n",
        e.roll,
        e.pitch,
        e.yaw
    );
}

void loop()
{
    
}