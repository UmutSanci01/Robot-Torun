#include "../../lib/Motor/Motor.h"

Motor::Motor(
    uint8_t in1,
    uint8_t in2,
    uint8_t enablePin,
    uint8_t pwmChannel
)
:
in1_(in1),
in2_(in2),
enablePin_(enablePin),
enabled_(false),
power_(0),
pwmChannel_(pwmChannel),
pwmFrequency_(20000),
pwmResolution_(8)
{
}

bool Motor::begin()
{
    pinMode(in1_, OUTPUT);
    pinMode(in2_, OUTPUT);
    pinMode(enablePin_, OUTPUT);
    
    ledcSetup(
        pwmChannel_,
        pwmFrequency_,
        pwmResolution_);

    ledcAttachPin(
        in1_,
        pwmChannel_);
    
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
    uint8_t duty = map(abs(power_), 0, 100, 0, 255);

    if (power_ > 0)
    {
      ledcDetachPin(in2_);

      ledcAttachPin(
          in1_,
          pwmChannel_);

      digitalWrite(in2_, LOW);

      ledcWrite(
          pwmChannel_,
          duty);
    }
    else if (power_ < 0)
    {
      ledcDetachPin(in1_);

      ledcAttachPin(
          in2_,
          pwmChannel_);

      digitalWrite(in1_, LOW);

      ledcWrite(
          pwmChannel_,
          duty);
    }
    else
    {
      ledcWrite(
          pwmChannel_,
          0);

      digitalWrite(in1_, LOW);
      digitalWrite(in2_, LOW);
    }
}

void Motor::stop()
{
    setPower(0);
}