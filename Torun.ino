#include "lib\Encoder\Encoder.h"

Encoder leftEncoder(25, 26);
Encoder rightEncoder(33, 32);

void setup()
{
    Serial.begin(115200);

    leftEncoder.begin();
    rightEncoder.begin();

    Serial.println("Encoder OK");
}

void loop()
{
    Serial.printf(
    "R:%ld   L:%ld\n",
    rightEncoder.position(),
    leftEncoder.position());

    delay(100);
}