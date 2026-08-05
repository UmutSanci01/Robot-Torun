#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Button.h"

class Menu
{
public:

    Menu(
        Button& btnUp,
        Button& btnSelect,
        Adafruit_SSD1306& display
    );

    bool begin();

    void update();

private:

    enum class State
    {
        MAIN,
        MOTOR_TEST,
        ENCODER_TEST,
        IMU_TEST
    };

    State state_;

    Button& btnUp_;
    Button& btnSelect_;
    Adafruit_SSD1306& display_;

    uint8_t cursor_;

    bool redraw_;

    void updateMainMenu();

    void drawMainMenu();
};