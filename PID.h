#pragma once

class PID
{
public:

    PID(float kp = 0.0f,
        float ki = 0.0f,
        float kd = 0.0f);

    void setTunings(
        float kp,
        float ki,
        float kd);

    void setOutputLimits(
        float minimum,
        float maximum);

    void setTarget(float target);

    void reset();

    float update(
        float input,
        float dt);

    float output() const;

private:

    float kp_;
    float ki_;
    float kd_;

    float target_;

    float integral_;
    float previousError_;

    float output_;

    float minOutput_;
    float maxOutput_;
};