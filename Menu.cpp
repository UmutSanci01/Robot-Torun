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
            updateEncoderTest();
            break;

        case State::PID_CALIBRATION:
            updatePIDCalibration();
            return;
            break;
        case State::IMU_TEST:
            updateIMUTest();
            break;
        case State::PATTERN_TEST:
            updatePatternTest();
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
    if(btnUp_.longPress)
    {
        page_++;
        if(page_ == pageNum_)
        {
            page_ = 0;
        }

        redraw_ = true;
    }

    if(btnSelect_.click)
    {
        drive_.disable();

        if (page_ == 0)
        {
            switch(cursor_)
            {
                case 0:
                    state_ = State::ROBOT_CONFIG;
                    redraw_ = true;
                    return;
                    break;

                case 1:
                    state_ = State::ENCODER_TEST;
                    return;
                    break;
                case 2:
                    state_ = State::IMU_TEST;
                    return;
                    break;
                case 3:
                    state_ = State::PID_CALIBRATION;
                    return;
                    break;
            }
        }
        
        if (page_ == 1)
        {
            switch(cursor_)
            {
                case 0:
                    state_ = State::PATTERN_TEST;
                    redraw_ = true;
                    return;
                    break; 
            }
        }
        redraw_ = true;
    }

    if(redraw_)
    {
        drawMainMenu();
        redraw_ = false;
    }
}

const char* items[][4] =
{
    {
        "Robot Config",
        "Encoder Test",
        "IMU Test",
        "PID Calibration"
    },
    {
        "Pattern Test",
        ".",
        ".",
        "."
    }
};
void Menu::drawMainMenu()
{
    display_.clearDisplay();

    display_.setTextSize(1);
    display_.setTextColor(SSD1306_WHITE);

    for(uint8_t i = 0; i < 4; i++)
    {
        display_.setCursor(0, i * 16);

        if(i == cursor_)
            display_.print("> ");
        else
            display_.print("  ");

        display_.println(items[page_][i]);
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

uint32_t turnDelay = .0f;
float targetDegree = .0f;
bool isTurning = false;
bool isTurningStart = false;
void Menu::drawIMUTest()
{
    display_.clearDisplay();

    display_.setCursor(0,0);
    display_.println("IMU Test");
    
    display_.printf("Kp: %.1f | Tg: %.1f\n", drive_.Kp_Degree, targetDegree);
    
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
            targetDegree -= 15.1f;

        redraw_=true;
    }

    if (btnUp_.click)
    {
        if (!isTurningStart)
            targetDegree += 15.1f;

        redraw_=true;
    }

    if (btnSelect_.longPress)
    {
        turnDelay = millis();
        isTurning = true;


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
        drive_.rotateIMU(targetDegree, imu_);

        if(!drive_.turning())
        {
            isTurning = false;
            isTurningStart = false;
        }
    }
    else if (isTurning && millis() - turnDelay > 2000)
    {
        isTurningStart = true;
        drive_.enable();
        drive_.stop();
    }

    if(redraw_)
    {
        drawIMUTest();
        redraw_ = false;
    }
}

float targetDistCM = .0f;
uint32_t driveDelay = .0f;
bool isDriveStart = false;
bool isDrive = false;
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

    display_.printf("targetDistance: %.2f\n", targetDistCM);

    if (isDrive)
        display_.println("Driving...");

    display_.display();
}

void Menu::updateEncoderTest()
{
    if(btnSelect_.click)
    {
        targetDistCM += 15.f;

        redraw_=true;
    }
    if(btnSelect_.longPress)
    {
        isDrive = true;
        isDriveStart = false;
        driveDelay = millis();

        // This variable is also used in the IMU Test
        targetDegree = imu_.orientation().yaw;

        // Reset encoder ticks
        drive_.leftEncoder().reset();
        drive_.rightEncoder().reset();

        redraw_=true;
    }
    if (btnUp_.click)
    {
        if (targetDistCM > 0.f)
            targetDistCM -= 15.f;

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

    if (isDriveStart)
    {
        if (drive_.driveDistanceIMU(targetDistCM, targetDegree, 20, imu_))
        {
            isDrive = false;
            isDriveStart = false;
        }
    }
    else if (isDrive && millis() - driveDelay > 2000)
    {
        isDriveStart = true;

        drive_.enable();
        drive_.stop();
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
float values[3] = {4.f, 12.f, 0.005f};
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
        if (!drive_.enabled())
        {
            drive_.enable();
        }

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


enum Pattern
{
    Square,
    Rectangle,
    Triangle,
    END
};

const char *pattern_text[] = {
    "Square",
    "Rectangle",
    "Triangle"
};

uint32_t patternDelay = 0;
bool isPattern = false;
bool isPatternStart = false;
int currPattern = Pattern::Square;

uint8_t patternStep = 0; 
uint8_t edgeCount = 0;   
void Menu::drawPatternTest()
{
    display_.clearDisplay();
    display_.setCursor(0, 0);
    display_.println("Pattern Test");

    display_.printf("Pattern: %s\n", pattern_text[currPattern]);

    if (isPattern)
        display_.println("Patterning...");

    display_.display();
}

void Menu::updatePatternTest()
{
    if (btnUp_.click)
    {
        currPattern++;
        if (currPattern == Pattern::END)
            currPattern = Pattern::Square;

        redraw_ = true;
    }

    if (btnSelect_.longPress)
    {
        patternDelay = millis();
        isPattern = true;
        isPatternStart = false;
        
        patternStep = 0;
        edgeCount = 0;
        targetDegree = imu_.orientation().yaw; 
        redraw_ = true;
    }

    if (btnUp_.longPress)
    {
        drive_.stop();
        drive_.disable();
        
        isPattern = false;
        isPatternStart = false;
        state_ = State::MAIN;
        redraw_ = true;
        return;
    }

    if (isPattern && !isPatternStart && (millis() - patternDelay > 2000))
    {
        isPatternStart = true;
        drive_.stop();
        drive_.enable();
    }

    if (isPatternStart)
    {
        float targetDistance = 0.0f;
        float turnAngle = 0.0f;
        uint8_t maxEdges = 0;
        float baseRPM = 55.0f; // Sürüş hızı sabiti

        if (currPattern == Pattern::Square) 
        {
            targetDistance = 30.0f;
            turnAngle = 90.0f;
            maxEdges = 4;
        } 
        else if (currPattern == Pattern::Rectangle) 
        {
            targetDistance = (edgeCount % 2 == 0) ? 45.0f : 20.0f;
            turnAngle = 90.0f;
            maxEdges = 4;
        } 
        else if (currPattern == Pattern::Triangle) 
        {
            targetDistance = 30.0f;
            turnAngle = 120.0f;
            maxEdges = 3;
        }

        if (patternStep == 0) 
        {
            drive_.leftEncoder().reset(); 
            drive_.rightEncoder().reset();
            
            drive_.enable();

            patternStep = 1;
        }
        else if (patternStep == 1) 
        {
            if (drive_.driveDistanceIMU(targetDistance, targetDegree, baseRPM, imu_)) 
            {
                patternStep = 2; 
            }
        }
        else if (patternStep == 2) 
        {
            drive_.enable();
            targetDegree += turnAngle;
            
            while (targetDegree > 180.0f) targetDegree -= 360.0f;
            while (targetDegree < -180.0f) targetDegree += 360.0f;
            
            patternStep = 3;
        }
        else if (patternStep == 3) 
        {
            drive_.rotateIMU(targetDegree, imu_);
            
            if (!drive_.turning()) 
            {
                edgeCount++;
                if (edgeCount >= maxEdges) 
                {
                    edgeCount = 0;
                }
                patternStep = 0;
            }
        }
    }

    if (redraw_)
    {
        drawPatternTest();
        redraw_ = false;
    }
}