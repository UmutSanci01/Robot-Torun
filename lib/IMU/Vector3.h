#pragma once

struct Vector3f
{
    float x;
    float y;
    float z;

    constexpr Vector3f()
        : x(0.0f), y(0.0f), z(0.0f)
    {
    }

    constexpr Vector3f(float X, float Y, float Z)
        : x(X), y(Y), z(Z)
    {
    }

    void clear()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
};