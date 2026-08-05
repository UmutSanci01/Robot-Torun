#include "Menu.h"

Menu::Menu(Button& btnUp, Button& btnSelect)
:
btnUp_(btnUp),
btnSelect_(btnSelect),
state_(State::MAIN),
cursor_(0)
{
}

bool Menu::begin()
{
    return true;
}

void Menu::update()
{
    switch(state_)
    {
        case State::MAIN:
            updateMainMenu();
            break;

        default:
            break;
    }
}

void Menu::updateMainMenu()
{
    if(btnUp_.click)
    {
        cursor_++;

        if(cursor_ > 2)
            cursor_ = 0;

        drawMainMenu();
    }

    if(btnSelect_.click)
    {
        switch(cursor_)
        {
            case 0:
                state_ = State::MOTOR_TEST;
                break;

            case 1:
                state_ = State::ENCODER_TEST;
                break;

            case 2:
                state_ = State::IMU_TEST;
                break;
        }
    }
}

void Menu::drawMainMenu()
{
    // OLED kodu Commit 015'te gelecek.
}