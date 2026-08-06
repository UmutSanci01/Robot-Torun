#include "Menu.h"

Menu::Menu(
    Button& btnUp,
    Button& btnSelect,
    Adafruit_SSD1306& display,
    Drive& drive
)
:
btnUp_(btnUp),
btnSelect_(btnSelect),
display_(display),
drive_(drive),
state_(State::MAIN),
cursor_(0),
redraw_(true),
lastRefreshMs_(0)
{
}

bool Menu::begin()
{
    redraw_ = false;
    
    lastRefreshMs_ = millis();

    drawMainMenu();
    return true;
}

void Menu::update()
{
    uint32_t now = millis();

    if (now - lastRefreshMs_ >= REFRESH_INTERVAL_MS)
    {
        lastRefreshMs_ = now;
        redraw_ = true;
    }

    switch(state_)
    {
        case State::MAIN:
            updateMainMenu();
            break;

        case State::MOTOR_TEST:
            updateMotorTest();
            break;
        case State::ENCODER_TEST:
            state_ = State::MAIN;
            redraw_ = true;
            return;
            break;

        case State::IMU_TEST:
            state_ = State::MAIN;
            redraw_ = true;
            return;
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

        redraw_ = true;
    }

    if(btnSelect_.click)
    {
        switch(cursor_)
        {
            case 0:
                state_ = State::MOTOR_TEST;
                redraw_ = true;
                return;
                break;

            case 1:
                state_ = State::ENCODER_TEST;
                return;
                break;

            case 2:
                state_ = State::IMU_TEST;
                return;
                break;
        }

        redraw_ = true;
    }

    if(redraw_)
    {
        drawMainMenu();
        redraw_ = false;
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

void Menu::drawMotorTest()
{
    display_.clearDisplay();

    display_.setCursor(0,0);
    display_.println("Motor Test");

    display_.println();

    display_.print("Driver : ");

    if(drive_.enabled())
        display_.println("ON");
    else
        display_.println("OFF");

    display_.println();

    display_.print("L:");
    display_.println(drive_.leftDistance(),3);

    display_.print("R:");
    display_.println(drive_.rightDistance(),3);

    display_.display();
}

void Menu::updateMotorTest()
{
    if(btnSelect_.click)
    {
        if(!drive_.enabled())
        {
            drive_.enable();
        }

        int p = drive_.power();

        if(p<100)
            p+=5;

        drive_.setPower(p);

        redraw_=true;
    }

    if(btnUp_.longPress)
    {
        drive_.stop();

        drive_.disable();

        state_=State::MAIN;

        redraw_=true;

        return;
    }

    if(redraw_)
    {
        drawMotorTest();
        redraw_ = false;
    }
}

void Menu::drawMotorPower()
{
  display_.clearDisplay();

  display_.setCursor(0,0);
  display_.println("Motor Test");

  display_.println();

  display_.print("Power : ");
  display_.println(drive_.power());

  display_.println();

  display_.println("BTN1 -");

  display_.println("BTN2 +");

  display_.display();
}