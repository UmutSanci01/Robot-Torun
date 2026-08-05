#include "lib/Motor/Motor.h"
#include "MotorConfig.h"
#include "Button.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED width in pixels
#define SCREEN_HEIGHT 64 // OLED height in pixels
#define OLED_RESET -1 // No reset pin
#define SCREEN_ADDRESS 0x3C // I2C address of the OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Motor leftMotor(26, 25, 14, 0);
Motor rightMotor(32, 33, 14, 1);

Button btn1(18);
Button btn2(5);

void check_button();

void setup()
{
    Serial.begin(115200);
    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("OLED bulunamadi!");
        while (1);
    }

    leftMotor.begin();
    rightMotor.begin();

    leftMotor.enable();
    rightMotor.enable();

    display.clearDisplay();
    display.setTextSize(1); // Text size
    display.setTextColor(WHITE); // Text color
    display.setCursor(0, 0); // Start position

    btn1.begin();
    btn2.begin();
    
    digitalWrite(14, LOW);
}

void loop()
{
    check_button();
    leftMotor.setPower(100);
    rightMotor.setPower(100);
    delay(30);
    for(int p=30;p<=100;p+=10)
    {
        check_button();
        leftMotor.setPower(p);
        rightMotor.setPower(p);

        // Serial.printf("Power %d\n",p);
        display.clearDisplay();
        display.setCursor(0,0);
        display.printf("Power %d\n", p);
        display.display();


        delay(2000);
    }

    delay(1000);

    leftMotor.stop();
    rightMotor.stop();
    display.clearDisplay();

    delay(1000);
}

void check_button()
{
    btn1.update();
    if (btn1.down)
    {
        if (leftMotor.enabled())
        {
            display.printf("Motorlar\nAktif\n");
            leftMotor.disable();
            rightMotor.disable();
        }
        else
        {
            display.printf("Motorlar\Devredisi\n");
            leftMotor.enable();
            rightMotor.enable();
        }
    }
}