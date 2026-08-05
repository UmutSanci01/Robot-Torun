#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button
{
public:

    bool down = false;
    bool up = false;
    bool click = false;
    bool longPress = false;
    bool pressed = false;

    Button(uint8_t pin, uint16_t debounce = 40, uint16_t longTime = 800)
    {
        _pin = pin;
        _debounce = debounce;
        _longTime = longTime;
    }

    void begin()
    {
        pinMode(_pin, INPUT_PULLUP);

        _lastReading = digitalRead(_pin);
        _state = _lastReading;
    }

    void update()
    {
        down = false;
        up = false;
        click = false;
        longPress = false;

        bool reading = digitalRead(_pin);

        if(reading != _lastReading)
        {
            _lastDebounce = millis();
        }

        if((millis() - _lastDebounce) > _debounce)
        {
            if(reading != _state)
            {
                _state = reading;

                if(_state == LOW)
                {
                    down = true;
                    pressed = true;

                    _pressTime = millis();
                    _longSent = false;
                }
                else
                {
                    up = true;
                    pressed = false;

                    if(!_longSent)
                        click = true;
                }
            }
        }

        if(pressed && !_longSent)
        {
            if(millis() - _pressTime >= _longTime)
            {
                longPress = true;
                _longSent = true;
            }
        }

        _lastReading = reading;
    }

private:

    uint8_t _pin;

    bool _state;
    bool _lastReading;

    bool _longSent = false;

    unsigned long _lastDebounce = 0;
    unsigned long _pressTime = 0;

    uint16_t _debounce;
    uint16_t _longTime;
};

#endif