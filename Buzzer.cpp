#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin) :
pin_(pin)
{
  pinMode(pin, OUTPUT);
  initialized_ = true;
}
void Buzzer::beep(uint32_t millisecond)
{
  if (!healthy()) return;
  digitalWrite(pin_, HIGH);
  delay(millisecond);
  digitalWrite(pin_, LOW);
}
bool Buzzer::healthy()
{
  return initialized_;
}