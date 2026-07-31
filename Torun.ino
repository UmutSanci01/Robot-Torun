#include "lib\Encoder\Encoder.h"

Encoder leftEncoder(32, 33);
Encoder rightEncoder(25, 26);

void setup()
{
    Serial.begin(115200);

    leftEncoder.begin();
    rightEncoder.begin();

    Serial.println("Encoder OK");
}

void loop()
{
}