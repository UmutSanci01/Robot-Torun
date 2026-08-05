#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Button.h"
#include "lib\Motor\Motor.h"

class Menu
{
public:

    Menu(
        Button& btnUp,
        Button& btnSelect,
        Adafruit_SSD1306& display,
        Motor& leftMotor,
        Motor& rightMotor
    );

    bool begin();

    void update();

    void updateMotorTest();

    void drawMotorTest();

private:

  enum class State
  {
      MAIN,
      MOTOR_TEST,
      MOTOR_ENABLE,
      MOTOR_PWM,
      ENCODER_TEST,
      IMU_TEST
  };

    State state_;

    Button& btnUp_;
    Button& btnSelect_;
    Adafruit_SSD1306& display_;

    Motor& leftMotor_;
    Motor& rightMotor_;

    uint8_t cursor_;

    bool redraw_;

    void updateMainMenu();

    void drawMainMenu();
};