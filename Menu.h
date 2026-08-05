#pragma once

#include <Arduino.h>
#include "Button.h"

class Menu
{
public:

    Menu(Button& btnUp, Button& btnSelect);

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

    uint8_t cursor_;

    void updateMainMenu();

    void drawMainMenu();
};