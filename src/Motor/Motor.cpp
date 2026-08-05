#include "../../lib/Motor/Motor.h"

Motor::Motor(
    uint8_t in1,
    uint8_t in2,
    uint8_t enablePin
)
:
in1_(in1),
in2_(in2),
enablePin_(enablePin),
enabled_(false),
power_(0)
{
}

bool Motor::begin()
{
    pinMode(in1_, OUTPUT);
    pinMode(in2_, OUTPUT);
    pinMode(enablePin_, OUTPUT);

    digitalWrite(in1_, LOW);
    digitalWrite(in2_, LOW);

    disable();

    return true;
}

void Motor::enable()
{
    digitalWrite(enablePin_, HIGH);
    enabled_ = true;
}

void Motor::disable()
{
    digitalWrite(enablePin_, LOW);
    enabled_ = false;
}

bool Motor::enabled() const
{
    return enabled_;
}

void Motor::setPower(int8_t power)
{
    if (!enabled_)
        return;

    power_ = constrain(power, -100, 100);

    if (power_ > 0)
    {
        digitalWrite(in1_, HIGH);
        digitalWrite(in2_, LOW);
    }
    else if (power_ < 0)
    {
        digitalWrite(in1_, LOW);
        digitalWrite(in2_, HIGH);
    }
    else
    {
        digitalWrite(in1_, LOW);
        digitalWrite(in2_, LOW);
    }
}

void Motor::stop()
{
    setPower(0);
}