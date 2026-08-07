#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Button.h"
#include "lib\Motor\Drive.h"

class Menu
{
public:

    Menu(
        Button& btnUp,
        Button& btnSelect,
        Adafruit_SSD1306& display,
        Drive& drive
    );

    bool begin();

    void update();

    void updateMotorTest();

    void drawMotorTest();

    void drawMotorPower();
    void updateEncoderSpeedTest();
    
    void drawEncoderTest();
    void updateEncoderTest();

void drawEncoderSpeedTest();

private:

  enum class State
  {
      MAIN,
      MOTOR_TEST,
      MOTOR_ENABLE,
      MOTOR_PWM,
      ENCODER_TEST,
      IMU_TEST,
      ENCODER_SPEED_TEST
  };

    State state_;

    Button& btnUp_;
    Button& btnSelect_;
    Adafruit_SSD1306& display_;

    Drive& drive_;

    uint8_t cursor_;

    bool redraw_;

    uint32_t lastRefreshMs_;
    static constexpr uint16_t REFRESH_INTERVAL_MS = 100;

    void updateMainMenu();

    void drawMainMenu();
};