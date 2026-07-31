#include "..\..\lib\Encoder\Encoder.h"

Encoder* Encoder::instance0_ = nullptr;
Encoder* Encoder::instance1_ = nullptr;

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
distance_(0.0f),
instanceIndex_(255),
previousState_(0)
{
}

bool Encoder::begin()
{
    pinMode(pinA_, INPUT_PULLUP);
    pinMode(pinB_, INPUT_PULLUP);

    previousState_ =
        (digitalRead(pinA_) << 1) |
        digitalRead(pinB_);

    if (instance0_ == nullptr)
    {
        instance0_ = this;
        instanceIndex_ = 0;

        attachInterrupt(
            digitalPinToInterrupt(pinA_),
            isr0,
            CHANGE);

        attachInterrupt(
            digitalPinToInterrupt(pinB_),
            isr0,
            CHANGE);
    }
    else
    {
        instance1_ = this;
        instanceIndex_ = 1;

        attachInterrupt(
            digitalPinToInterrupt(pinA_),
            isr1,
            CHANGE);

        attachInterrupt(
            digitalPinToInterrupt(pinB_),
            isr1,
            CHANGE);
    }

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

void IRAM_ATTR Encoder::isr0()
{
    if(instance0_)
        instance0_->handleInterrupt();
}

void IRAM_ATTR Encoder::isr1()
{
    if(instance1_)
        instance1_->handleInterrupt();
}

void IRAM_ATTR Encoder::handleInterrupt()
{
    uint8_t state =
        (digitalRead(pinA_) << 1) |
        digitalRead(pinB_);

    uint8_t transition =
        (previousState_ << 2) | state;

    switch (transition)
    {
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            position_++;
            break;

        case 0b0010:
        case 0b0100:
        case 0b1101:
        case 0b1011:
            position_--;
            break;

        default:
            break;
    }

    previousState_ = state;
}