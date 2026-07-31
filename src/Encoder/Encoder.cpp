#include "..\..\lib\Encoder\Encoder.h"

Encoder::Encoder(
    uint8_t pinA,
    uint8_t pinB
)
:
pinA_(pinA),
pinB_(pinB),
initialized_(false),
position_(0),
previousPosition_(0),
delta_(0),
rpm_(0.0f),
velocity_(0.0f),
distance_(0.0f)
{
}

bool Encoder::begin()
{
    pinMode(pinA_, INPUT_PULLUP);
    pinMode(pinB_, INPUT_PULLUP);

    initialized_ = true;
    return true;
}

bool Encoder::update()
{
    if (!initialized_)
    {
        return false;
    }

    return true;
}

bool Encoder::healthy() const
{
    return initialized_;
}

int32_t Encoder::position() const
{
    return position_;
}

int32_t Encoder::delta() const
{
    return delta_;
}

float Encoder::rpm() const
{
    return rpm_;
}

float Encoder::velocity() const
{
    return velocity_;
}

float Encoder::distance() const
{
    return distance_;
}

void Encoder::reset()
{
    position_ = 0;
    previousPosition_ = 0;
    delta_ = 0;
    rpm_ = 0.0f;
    velocity_ = 0.0f;
    distance_ = 0.0f;
}