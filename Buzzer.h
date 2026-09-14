#pragma once

#include <Arduino.h>

class Buzzer
{
  public:
    Buzzer(uint8_t pin);
    void beep(uint32_t millisecond);
    bool healthy();

  private:
    uint8_t pin_;
    bool initialized_;
};

// #ifndef BUZZER_H
// #define BUZZER_H



// #endif