#pragma once

struct Euler
{
    float roll;
    float pitch;
    float yaw;

    constexpr Euler()
        : roll(0.0f), pitch(0.0f), yaw(0.0f)
    {
    }

    constexpr Euler(float r, float p, float y)
        : roll(r), pitch(p), yaw(y)
    {
    }
};