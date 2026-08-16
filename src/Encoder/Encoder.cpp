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
previousState_(0),
previousUpdateMs_(0)
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

    previousUpdateMs_ = millis();
    initialized_ = true;

    return true;
}

bool Encoder::update()
{
    if (!initialized_)
        return false;

    uint32_t now = millis();
    uint32_t dt = now - previousUpdateMs_;

    if (dt >= 20)
    {
        int32_t current = ticks();

        delta_ = current - previousPosition_;
        previousPosition_ = current;

        float rev = (float)delta_ / ticksPerRevolution_;

        rpm_ = rev * (60000.0f / dt);

        velocity_ = rpm_ * (PI * wheelDiameter_) / 60.0f;

        previousUpdateMs_ = now;
    }

    return true;
}

bool Encoder::healthy() const
{
    return initialized_;
}

int32_t Encoder::position() const
{
    noInterrupts();
    int32_t p = position_;
    interrupts();

    return p;
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

// float Encoder::distance() const
// {
//     return distance_;
// }

// void Encoder::reset()
// {
//     position_ = 0;
//     previousPosition_ = 0;
//     delta_ = 0;
//     rpm_ = 0.0f;
//     velocity_ = 0.0f;
//     distance_ = 0.0f;
// }

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
            count_++;
            break;

        case 0b0010:
        case 0b0100:
        case 0b1101:
        case 0b1011:
            count_--;
            break;

        default:
            break;
    }

    previousState_ = state;
}

int32_t Encoder::ticks() const
{
    return count_;
}

void Encoder::reset()
{
    noInterrupts();
    count_ = 0;
    interrupts();
}

float Encoder::revolutions() const
{
    if (ticksPerRevolution_ <= 0.0f)
        return 0.0f;

    return (float)ticks() / ticksPerRevolution_;
}

float Encoder::distance() const
{
    return revolutions() * PI * wheelDiameter_;
}


void Encoder::setTicksPerRevolution(float ticks)
{
    ticksPerRevolution_ = ticks;
}

void Encoder::setWheelDiameter(float diameter)
{
    wheelDiameter_ = diameter;
}

float Encoder::ticksPerRevolution() const
{
    return ticksPerRevolution_;
}

float Encoder::wheelDiameter() const
{
    return wheelDiameter_;
}