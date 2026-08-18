#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "lib\Motor\Motor.h"
#include "Button.h"
#include "Menu.h"
#include "lib\Encoder\Encoder.h"
#include "lib\IMU\IMU.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1);

Button btnUp(5);
Button btnSelect(18);

Motor leftMotor(26, 25, 14, 0);
Motor rightMotor(32, 33, 14, 1);
Encoder leftEncoder(34, 35);
Encoder rightEncoder(39, 36);


Drive drive(
    leftMotor,
    rightMotor,
    leftEncoder,
    rightEncoder
);

IMU imu;

Menu menu(
    btnUp,
    btnSelect,
    display,
    drive,
    imu
);

void setup()
{
    Serial.begin(115200);

    Wire.begin();
    Wire.setClock(400000);
    display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C);

    display.clearDisplay();
    display.display();

    btnUp.begin();
    btnSelect.begin();

    leftMotor.begin();
    rightMotor.begin();

    leftEncoder.begin();
    rightEncoder.begin();

    leftEncoder.setTicksPerRevolution(5925.0f);
    rightEncoder.setTicksPerRevolution(5925.0f);

    leftEncoder.setWheelDiameter(0.044f);
    rightEncoder.setWheelDiameter(0.044f);

    drive.begin();
    imu.begin();

    drive.setTargetRPM(0, 0);

    menu.begin();
}

void loop()
{
    btnUp.update();
    btnSelect.update();

    leftEncoder.update();
    rightEncoder.update();

    imu.update();
    drive.update();

    menu.update();
}