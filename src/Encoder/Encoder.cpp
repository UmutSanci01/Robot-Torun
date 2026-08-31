#include "..\..\lib\Encoder\Encoder.h"

Encoder* Encoder::instance0_ = nullptr;
Encoder* Encoder::instance1_ = nullptr;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, pcnt_unit_t unit)
    : pinA_(pinA), pinB_(pinB), pcntUnit_(unit), initialized_(false),
      previousUpdateMs_(0), ticksPerRevolution_(1.0f), wheelDiameter_(1.0f),
      previousPosition_(0), delta_(0), rpm_(0.0f), velocity_(0.0f),
      distance_(0.0f), overflowCount_(0)
{
}

// bool Encoder::begin()
// {
//     pinMode(pinA_, INPUT_PULLUP);
//     pinMode(pinB_, INPUT_PULLUP);

//     previousState_ =
//         (digitalRead(pinA_) << 1) |
//         digitalRead(pinB_);

//     if (instance0_ == nullptr)
//     {
//         instance0_ = this;
//         instanceIndex_ = 0;

//         attachInterrupt(
//             digitalPinToInterrupt(pinA_),
//             isr0,
//             CHANGE);

//         attachInterrupt(
//             digitalPinToInterrupt(pinB_),
//             isr0,
//             CHANGE);
//     }
//     else
//     {
//         instance1_ = this;
//         instanceIndex_ = 1;

//         attachInterrupt(
//             digitalPinToInterrupt(pinA_),
//             isr1,
//             CHANGE);

//         attachInterrupt(
//             digitalPinToInterrupt(pinB_),
//             isr1,
//             CHANGE);
//     }

//     previousUpdateMs_ = millis();
//     initialized_ = true;

//     return true;
// }

bool Encoder::begin()
{
    pinMode(pinA_, INPUT_PULLUP);
    pinMode(pinB_, INPUT_PULLUP);
    
    pcnt_config_t pcnt_config = {};
    
    pcnt_config.pulse_gpio_num = pinA_;
    pcnt_config.ctrl_gpio_num = pinB_;
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = pcntUnit_;
    pcnt_config.pos_mode = PCNT_COUNT_INC;
    pcnt_config.neg_mode = PCNT_COUNT_DEC;
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim = 30000;
    pcnt_config.counter_l_lim = -30000;
    pcnt_unit_config(&pcnt_config);

    pcnt_config.pulse_gpio_num = pinB_;
    pcnt_config.ctrl_gpio_num = pinA_;
    pcnt_config.channel = PCNT_CHANNEL_1;
    pcnt_config.pos_mode = PCNT_COUNT_DEC;
    pcnt_config.neg_mode = PCNT_COUNT_INC;
    // pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
    // pcnt_config.hctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE; 
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_unit_config(&pcnt_config);

    pcnt_set_filter_value(pcntUnit_, 100);
    pcnt_filter_enable(pcntUnit_);

    pcnt_event_enable(pcntUnit_, PCNT_EVT_H_LIM);
    pcnt_event_enable(pcntUnit_, PCNT_EVT_L_LIM);

    pcnt_isr_service_install(0);
    pcnt_isr_handler_add(pcntUnit_, pcntOverflowIsr, (void*)this);

    pcnt_counter_pause(pcntUnit_);
    pcnt_counter_clear(pcntUnit_);
    pcnt_counter_resume(pcntUnit_);

    previousUpdateMs_ = millis();
    initialized_ = true;

    return true;
}

bool Encoder::update()
{
    if (!initialized_) return false;

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


    // if (!initialized_)
    //     return false;

    // uint32_t now = millis();
    // uint32_t dt = now - previousUpdateMs_;

    // if (dt >= 20)
    // {
    //     int32_t current = ticks();

    //     delta_ = current - previousPosition_;
    //     previousPosition_ = current;

    //     float rev = (float)delta_ / ticksPerRevolution_;

    //     rpm_ = rev * (60000.0f / dt);

    //     velocity_ = rpm_ * (PI * wheelDiameter_) / 60.0f;

    //     previousUpdateMs_ = now;
    // }

    // return true;
}

bool Encoder::healthy() const
{
    return initialized_;
}

int32_t Encoder::position() const
{
    int16_t hwCount = 0;
    pcnt_get_counter_value(pcntUnit_, &hwCount);
    return overflowCount_ + hwCount;


    // noInterrupts();
    // // int32_t p = position_;
    // int32_t p = count_;
    // interrupts();

    // return p;
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

void IRAM_ATTR Encoder::pcntOverflowIsr(void *arg)
{
    Encoder* enc = static_cast<Encoder*>(arg);
    uint32_t status;
    pcnt_get_event_status(enc->pcntUnit_, &status);

    if (status & PCNT_EVT_H_LIM) {
        enc->overflowCount_ += 30000;
    } else if (status & PCNT_EVT_L_LIM) {
        enc->overflowCount_ -= 30000;
    }
}

int32_t Encoder::ticks() const
{
    return position();
    // return count_;
}

// void Encoder::reset()
// {
//     noInterrupts();
//     count_ = 0;
//     interrupts();
// }

void Encoder::reset()
{
    pcnt_counter_pause(pcntUnit_);
    pcnt_counter_clear(pcntUnit_);
    overflowCount_ = 0;
    
    previousPosition_ = 0;
    delta_ = 0;
    rpm_ = 0.0f;
    velocity_ = 0.0f;
    distance_ = 0.0f;
    
    pcnt_counter_resume(pcntUnit_);


    // noInterrupts();
    // count_ = 0;
    // interrupts();

    // position_ = 0;
    // previousPosition_ = 0;
    // delta_ = 0;
    // rpm_ = 0.0f;
    // velocity_ = 0.0f;
    // distance_ = 0.0f;
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