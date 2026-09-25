#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "lib\Motor\Motor.h"
#include "Button.h"
#include "Menu.h"
#include "lib\Encoder\Encoder.h"
#include "lib\IMU\IMU.h"
#include "Config.h"
#include "ToFSensor.h"
#include "Buzzer.h"
#include "QMC5883.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#if TARGET_ROBOT == 1
    SemaphoreHandle_t i2cMutex;
    #define LOCK_I2C() (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE)
    #define UNLOCK_I2C() xSemaphoreGive(i2cMutex)
    
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
    
    Motor leftMotor(26, 25, 14, 0);
    Motor rightMotor(32, 33, 14, 1);
    Encoder leftEncoder(34, 35, PCNT_UNIT_0);
    Encoder rightEncoder(39, 36, PCNT_UNIT_1);
    Button btnSelect(18);
#elif TARGET_ROBOT == 2
    TwoWire I2C_OLED = TwoWire(1);
    // TwoWire I2C_SENSORS = TwoWire(1);
    
    #define LOCK_I2C() (true)
    #define UNLOCK_I2C() 
    
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, -1);
    
    Motor leftMotor(25, 26, 14, 0); 
    Motor rightMotor(27, 14, 14, 1);
    Encoder leftEncoder(34, 35, PCNT_UNIT_0);
    Encoder rightEncoder(32, 33, PCNT_UNIT_1);
    Button btnSelect(23);

#endif

Button btnUp(5);
Buzzer buzzer(4);
ToFSensor frontToFSensor;
QMC5883 compass;

Drive drive(
    leftMotor,
    rightMotor,
    leftEncoder,
    rightEncoder
);

IMU imu;

Menu menu(
    btnUp,
    btnSelect,
    display,
    drive,
    imu,
    frontToFSensor,
    compass
);



TaskHandle_t ControlTaskHandle;

void ControlTask(void *pvParameters);

void setup()
{
    delay(1000); // It waits one second to ignore initial vibrations and better calibrate for IMU.
    Serial.begin(115200);

    #if TARGET_ROBOT == 1
        Wire.begin();
        Wire.setClock(400000);
        i2cMutex = xSemaphoreCreateMutex();
        
        frontToFSensor.begin();
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        imu.begin();
        compass.begin();

    #elif TARGET_ROBOT == 2
        I2C_OLED.begin(21, 22); 
        I2C_OLED.setClock(400000);

        display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 

        Wire.begin(18, 19);
        Wire.setClock(400000);
        
        imu.begin();        
        frontToFSensor.begin();
        compass.begin();
    #endif

    btnUp.begin();
    btnSelect.begin();

    leftMotor.begin();
    rightMotor.begin();

    leftEncoder.begin();
    rightEncoder.begin();

    leftEncoder.setTicksPerRevolution(5925.0f);
    rightEncoder.setTicksPerRevolution(5925.0f);

    leftEncoder.setWheelDiameter(0.04438f);
    rightEncoder.setWheelDiameter(0.04438f);

    if (!imu.calibrate())
    {
        Serial.println("IMU could not calibrate.");
    }
    
    drive.begin();
    drive.stop();
    drive.enable();
    drive.setPIDTunings(Config::kp, Config::ki, Config::kd);

    xTaskCreatePinnedToCore(
        ControlTask,
        "ControlTask", 
        4096,
        NULL,
        2,
        &ControlTaskHandle,
        0
    );

    buzzer.beep(5);

    menu.begin();
}

// void printOffsets()
// {
//     Serial.print("H_X "); Serial.print(compass.hardOffsetX);
//     Serial.print(" H_Y "); Serial.print(compass.hardOffsetY);
//     Serial.print(" H_Z "); Serial.println(compass.hardOffsetZ);

//     Serial.print("S_X "); Serial.print(compass.softScaleX);
//     Serial.print(" S_Y "); Serial.print(compass.softScaleY);
//     Serial.print(" S_Z "); Serial.println(compass.softScaleZ);
// }

void loop()
{
    btnUp.update();
    btnSelect.update();
    
    // if (btnUp.click) {
    //     if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    //         printOffsets();
    //         xSemaphoreGive(i2cMutex);
    //     }
    // }
    
    if (!drive.turning() && !drive.driving())
    {
        if (LOCK_I2C()) {
            menu.update();
            UNLOCK_I2C();
        }
    }
    else
    {
        menu.update(false);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}

// void printMangneto()
// {
//     Serial.print("M_X "); Serial.print(compass.getX());
//     Serial.print(" M_Y "); Serial.print(compass.getY());
//     Serial.print(" M_Z "); Serial.println(compass.getZ());
// }

void ControlTask(void *pvParameters) {
    for(;;) {
        leftEncoder.update();
        rightEncoder.update();

        if (LOCK_I2C()) {
            imu.update();
            compass.update();
            // printMangneto();
            frontToFSensor.update();

            UNLOCK_I2C();
        }
        

        drive.update();

        vTaskDelay(pdMS_TO_TICKS(5)); 
    }
}