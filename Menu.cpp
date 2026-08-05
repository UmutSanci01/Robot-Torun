#include "Menu.h"

Menu::Menu(
    Button& btnUp,
    Button& btnSelect,
    Adafruit_SSD1306& display
)
:
btnUp_(btnUp),
btnSelect_(btnSelect),
display_(display),
state_(State::MAIN),
cursor_(0),
redraw_(true)
{
}

bool Menu::begin()
{
    drawMainMenu();
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

    if(redraw_)
    {
        drawMainMenu();
        redraw_ = false;
    }
}

void Menu::updateMainMenu()
{
    if(btnUp_.click)
    {
        cursor_++;

        if(cursor_ > 2)
            cursor_ = 0;

        redraw_ = true;
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

        redraw_ = true;
    }
}

void Menu::drawMainMenu()
{
    display_.clearDisplay();

    display_.setTextSize(1);
    display_.setTextColor(SSD1306_WHITE);

    const char* items[] =
    {
        "Motor Test",
        "Encoder Test",
        "IMU Test"
    };

    for(uint8_t i = 0; i < 3; i++)
    {
        display_.setCursor(0, i * 16);

        if(i == cursor_)
            display_.print("> ");
        else
            display_.print("  ");

        display_.println(items[i]);
    }

    display_.display();
}