#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Button.h"
#include "lib\Motor\Drive.h"
#include "lib\IMU\IMU.h"
#include "Config.h"

class Menu
{
public:

    Menu(
        Button& btnUp,
        Button& btnSelect,
        Adafruit_SSD1306& display,
        Drive& drive,
        IMU& imu
    );

    bool begin();

    // void update();
    void update(bool draw=true);

    void updateRobotConfig();

    void drawRobotConfig();

    // void drawMotorPower();

    void updateIMUTest();
    void drawIMUTest();
    
    void drawEncoderTest();
    void updateEncoderTest();

    void drawPIDCalibration();
    void updatePIDCalibration();

    void drawPatternTest();
    void updatePatternTest();

private:

  enum class State
  {
      MAIN,
      ROBOT_CONFIG,
      MOTOR_ENABLE,
      MOTOR_PWM,
      ENCODER_TEST,
      PID_CALIBRATION,
      IMU_TEST,
      PATTERN_TEST
  };

    State state_;

    Button& btnUp_;
    Button& btnSelect_;
    Adafruit_SSD1306& display_;

    Drive& drive_;
    IMU& imu_;

    uint8_t cursor_;
    uint8_t page_;
    uint8_t pageNum_ = 2;

    bool redraw_;

    uint32_t lastRefreshMs_;
    static constexpr uint16_t REFRESH_INTERVAL_MS = 500;

    int pidTargetRPM_ = 0;

    void updateMainMenu();

    void drawMainMenu();
};