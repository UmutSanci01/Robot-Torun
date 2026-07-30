class Mahony
{
public:

    void reset();

    void update(
        float gx,
        float gy,
        float gz,

        float ax,
        float ay,
        float az,

        float dt);

    Euler euler() const;

private:

    float q0 = 1.0f;
    float q1 = 0.0f;
    float q2 = 0.0f;
    float q3 = 0.0f;

};