#include "Menu.h"

Menu::Menu(
    Button& btnUp,
    Button& btnSelect,
    Adafruit_SSD1306& display,
    Drive& drive,
    IMU& imu
)
:
btnUp_(btnUp),
btnSelect_(btnSelect),
display_(display),
drive_(drive),
imu_(imu),
state_(State::MAIN),
cursor_(0),
redraw_(true),
lastRefreshMs_(0)
{
}

bool Menu::begin()
{
    redraw_ = false;
    
    lastRefreshMs_ = millis();

    drawMainMenu();
    return true;
}

void Menu::update()
{
    uint32_t now = millis();

    if (now - lastRefreshMs_ >= REFRESH_INTERVAL_MS)
    {
        lastRefreshMs_ = now;
        redraw_ = true;
    }

    switch(state_)
    {
        case State::MAIN:
            updateMainMenu();
            break;

        case State::ROBOT_CONFIG:
            updateRobotConfig();
            break;
        case State::ENCODER_TEST:
            state_ = State::ENCODER_TEST;
            updateEncoderTest();
            break;

        case State::PID_CALIBRATION:
            drive_.enable();
            updatePIDCalibration();
            return;
            break;
        case State::IMU_TEST:
            updateIMUTest();
            break;
        default:
            break;
    }
}

void Menu::updateMainMenu()
{
    if(btnUp_.click)
    {
        cursor_++;

        if(cursor_ > 3)
            cursor_ = 0;

        redraw_ = true;
    }

    if(btnSelect_.click)
    {
        switch(cursor_)
        {
            case 0:
                state_ = State::ROBOT_CONFIG;

                // drive_.setPIDTunings(2.0f, 0.0f, 0.0f);
                // drive_.setTargetRPM(50, 50);
                drive_.enable();

                redraw_ = true;
                return;
                break;

            case 1:
                state_ = State::ENCODER_TEST;
                return;
                break;
            case 2:
                state_ = State::IMU_TEST;
                drive_.enable();
                return;
                break;
            case 3:
                state_ = State::PID_CALIBRATION;
                drive_.enable();
                return;
                break;
        }

        redraw_ = true;
    }

    if(redraw_)
    {
        drawMainMenu();
        redraw_ = false;
    }
}

void Menu::drawMainMenu()
{
    display_.clearDisplay();

    display_.setTextSize(1);
    display_.setTextColor(SSD1306_WHITE);

    const char* items[] =
    {
        "Robot Config",
        "Encoder Test",
        "IMU Test",
        "PID Calibration"
    };

    for(uint8_t i = 0; i < 4; i++)
    {
        display_.setCursor(0, i * 16);

        if(i == cursor_)
            display_.print("> ");
        else
            display_.print("  ");

        display_.println(items[i]);
    }

    display_.display();
}

float rightRPM = 0.f, leftRPM = 0.f;
void Menu::drawRobotConfig()
{
    display_.clearDisplay();

    display_.setCursor(0,0);
    display_.println("Robot Configure");

    display_.println();

    display_.print("Target RPM ");
    display_.print(leftRPM, 0);
    display_.print(' ');
    display_.println(rightRPM, 0);

    // display_.println("Current RPMs");
    // display_.print("R ");
    // display_.println(drive_.rightEncoder().rpm());
    // display_.print("L ");
    // display_.println(drive_.leftEncoder().rpm());


    // display_.print("L:");
    // display_.print(drive_.leftEncoder().distance());
    // display_.println(" meter");

    // display_.print("R:");
    // display_.print(drive_.rightEncoder().distance());
    // display_.println(" meter");
    // display_.print("PL:");
    // display_.println(drive_.leftPIDOutput());

    // display_.print("PR:");
    // display_.println(drive_.rightPIDOutput());

    // display_.print("L ");
    // display_.print(drive_.leftEncoder().rpm(),1);
    // display_.println(" rpm");

    // display_.print("R ");
    // display_.print(drive_.rightEncoder().rpm(),1);
    // display_.println(" rpm");

    // display_.println();    
    // display_.print("Power ");
    // display_.print(drive_.power());
    // display_.println();

    display_.display();
}

void Menu::updateRobotConfig()
{
    if(btnSelect_.click)
    {
        if (leftRPM < 100)
        {
            leftRPM += 5;
            rightRPM += 5;
        }
        // if(!drive_.enabled())
        // {
        //     drive_.enable();
        // }

        // int p = drive_.power();

        // if(p<100)
        //     p+=1;

        // drive_.setPower(p);

        redraw_=true;
    }

    if (btnUp_.click)
    {
        if (leftRPM > 0)
        {
            leftRPM -= 5;
            rightRPM -= 5;
        }
        // if(!drive_.enabled())
        // {
        //     drive_.enable();
        // }

        // int p = drive_.power();

        // if(p<100)
        //     p-=1;

        // drive_.setPower(p);

        redraw_=true;
    }

    if(btnUp_.longPress)
    {
        drive_.stop();

        drive_.disable();

        state_=State::MAIN;

        redraw_=true;

        drive_.setTargetRPM(leftRPM, rightRPM);

        return;
    }

    // drive_.setPower(leftRPM, rightRPM);

    if(redraw_)
    {
        drawRobotConfig();
        redraw_ = false;
    }
}

float Kp_Degree = 1.0f;
bool isTurning = false;
bool isTurningStart = false;
void rotateIMU(float _targetDegree, IMU& imu, Drive& drive) {
    float currDegree = imu.orientation().yaw; //
    float degreeErr = _targetDegree - currDegree;

    while (degreeErr > 180.0f) degreeErr -= 360.0f;
    while (degreeErr < -180.0f) degreeErr += 360.0f;

    float targetRPM = degreeErr * Kp_Degree;

    targetRPM = constrain(targetRPM, -75.0f, 75.0f);

    if (abs(degreeErr) < .1f) {
        // drive.setTargetRPM(0, 0);
        drive.stop();
        drive.disable();

        isTurningStart = false;
        isTurning = false;
    } else {
        drive.setTargetRPM(targetRPM, -targetRPM);
    }
}

uint32_t turnDelay = .0f;
float targetDegree = .0f;
void Menu::drawIMUTest()
{
    display_.clearDisplay();

    display_.setCursor(0,0);
    display_.println("IMU Test");
    
    display_.printf("Kp: %.1f | Tg: %.1f\n", Kp_Degree, targetDegree);
    
    display_.printf("Roll : %.1f\n", imu_.orientation().roll);
    display_.printf("Pitch: %.1f\n", imu_.orientation().pitch);
    display_.printf("Yaw  : %.1f\n", imu_.orientation().yaw);

    display_.println();

    if (isTurning)
    {
        display_.println("TURNING...");
    }

    display_.display();
}

void Menu::updateIMUTest()
{
    if(btnSelect_.click)
    {
        if (!isTurningStart)
            targetDegree -= .1f;

        redraw_=true;
    }

    if (btnUp_.click)
    {
        if (!isTurningStart)
            targetDegree += .1f;

        redraw_=true;
    }

    if (btnSelect_.longPress)
    {
        turnDelay = millis();
        isTurning = true;
        drive_.enable();

        redraw_ = true;
    }

    if(btnUp_.longPress)
    {
        drive_.stop();
        drive_.disable();

        isTurning = false;
        isTurningStart = false;

        state_ = State::MAIN;
        redraw_ = true;
        return;
    }

    if (isTurningStart)
    {
        rotateIMU(targetDegree, imu_, drive_);
    }
    else if (isTurning && millis() - turnDelay > 2000)
    {
        isTurningStart = true;
    }

    if(redraw_)
    {
        drawIMUTest();
        redraw_ = false;
    }
}

void Menu::drawEncoderTest()
{
    display_.clearDisplay();

    display_.setCursor(0,0);
    display_.println("Encoder Test");

    display_.print("L ");
    display_.print(drive_.leftEncoder().ticks());
    display_.println(" tick");

    display_.print("R ");
    display_.print(drive_.rightEncoder().ticks());
    display_.println(" tick");

    display_.print("Power ");
    display_.print(drive_.power());
    display_.println();

    display_.display();
}

void Menu::updateEncoderTest()
{
        if(btnSelect_.click)
    {
        if(!drive_.enabled())
        {
            drive_.enable();
        }

        int p = drive_.power();

        if(p<100)
            p+=5;

        drive_.setPower(p);

        redraw_=true;
    }
    if(btnUp_.longPress)
    {
        drive_.stop();
        drive_.disable();

        state_ = State::MAIN;
        redraw_ = true;
        return;
    }

    if(redraw_)
    {
        drawEncoderTest();
        redraw_ = false;
    }
}

enum PIDValue
{
    kp,
    ki,
    kd
};
float values[3] = {0.f, 0.0f, 0.f};
int value_counter = 0;
void Menu::drawPIDCalibration()
{
    display_.clearDisplay();
    display_.setCursor(0,0);
    display_.print("PID Calibration\n");

    if (value_counter == 0)
        display_.print("> ");
    display_.print("Kp: ");
    display_.println(values[PIDValue::kp], 1);
    if (value_counter == 1)
        display_.print("> ");
    display_.print("Ki: ");   
    display_.println(values[PIDValue::ki], 0);
    if (value_counter == 2)
        display_.print("> ");
    display_.print("Kd: ");
    display_.println(values[PIDValue::kd], 3); 

    display_.print("L ");
    display_.print(drive_.leftEncoder().rpm(),0);
    display_.println(" rpm");

    display_.print("R ");
    display_.print(drive_.rightEncoder().rpm(),0);
    display_.println(" rpm");

    display_.print("L Out: ");
    display_.println(drive_.leftPIDOutput(), 3);

    display_.print("R Out: ");
    display_.println(drive_.rightPIDOutput(), 3);
    // if (value_counter == 3)
    //     display_.print("> ");
    // display_.print("Power: ");
    // display_.println(drive_.power());

    display_.display();
    redraw_ = true;
}

void Menu::updatePIDCalibration()
{
    bool tuningsChanged = false;

    if (btnUp_.click)
    {
        switch (value_counter)
        {
            case 0:
                values[PIDValue::kp] += 0.5f;
                break;
            case 1:
                // values[PIDValue::ki] += 0.01f;
                values[PIDValue::ki] += 2.f;
                break;
            case 2:
                values[PIDValue::kd] += 0.001f;
                break;
            // case 3:
            // {
            //     int p = drive_.power();

            //     if (p > -100)
            //         p -= 1;

            //     drive_.setPower(p);
            //     break;
            // }
            default:
                break;
        }
        tuningsChanged = true;
        redraw_ = true;
    }

    if (btnSelect_.click)
    {
        switch (value_counter)
        {
            case 0:
                if (values[PIDValue::kp] > 0)
                    values[PIDValue::kp] -= 0.5f;
                break;
            case 1:
                if (values[PIDValue::ki] > 0)
                    // values[PIDValue::ki] -= 0.01f;
                    values[PIDValue::ki] -= 2.f;
                break;
            case 2:
                if (values[PIDValue::kd] > 0)
                    values[PIDValue::kd] -= 0.001f;
                break;
            // case 3:
            // {
            //     int p = drive_.power();

            //     if (p < 100)
            //         p += 1;

            //     drive_.setPower(p);
            //     redraw_ = true;
            //     break;
            // }
            default:
                break;
        }
        tuningsChanged = true;
        redraw_ = true;
    }

    if (btnSelect_.longPress)
    {
        value_counter++;
        if (value_counter > 2)
            value_counter = 0;

        redraw_ = true;
    }

    if (btnUp_.longPress)
    {
        drive_.stop();
        drive_.disable();

        state_ = State::MAIN;
        redraw_ = true;
        return;
    }

    if (tuningsChanged) 
    {
        drive_.setPIDTunings(
            values[PIDValue::kp],
            values[PIDValue::ki],
            values[PIDValue::kd]
        );
    }

    if(redraw_)
    {
        drawPIDCalibration();
        redraw_ = false;
    }
}