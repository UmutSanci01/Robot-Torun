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

SemaphoreHandle_t i2cMutex;

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1);

Button btnUp(5);
Button btnSelect(18);
Buzzer buzzer(4);

Motor leftMotor(26, 25, 14, 0);
Motor rightMotor(32, 33, 14, 1);
Encoder leftEncoder(34, 35, PCNT_UNIT_0);
Encoder rightEncoder(39, 36, PCNT_UNIT_1);
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
    frontToFSensor
);



TaskHandle_t ControlTaskHandle;

void ControlTask(void *pvParameters);

void setup()
{
    delay(1000); // It waits one second to ignore initial vibrations and better calibrate for IMU.

    Serial.begin(115200);

    Wire.begin();
    Wire.setClock(400000);

    i2cMutex = xSemaphoreCreateMutex();

    if (!frontToFSensor.begin()) {
        Serial.println("There is no ToF Sensor!");
    }

    display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C);

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

    if (!imu.begin())
    {
        Serial.println("IMU not found.");
    }

    if (!imu.calibrate())
    {
        Serial.println("IMU could not calibrate.");
    }
    
    drive.begin();
    drive.stop();
    drive.enable();
    drive.setPIDTunings(Config::kp, Config::ki, Config::kd);

    compass.begin();

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

void loop()
{
    btnUp.update();
    btnSelect.update();
    
    // if (btnUp.click) {
    //     if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    //         ofsetleriYazdir();
    //         xSemaphoreGive(i2cMutex);
    //     }
    // }
    
    if (!drive.turning() && !drive.driving())
    {
        if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            menu.update();
            xSemaphoreGive(i2cMutex);
        }
    }
    else
    {
        menu.update(false);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}

void printMangneto()
{
    Serial.print("M_X "); Serial.print(compass.getX());
    Serial.print("M_Y "); Serial.print(compass.getY());
    Serial.print("M_Z "); Serial.println(compass.getZ());
}

void ControlTask(void *pvParameters) {
    for(;;) {
        leftEncoder.update();
        rightEncoder.update();

        if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            imu.update();
            compass.update();
            printMangneto();
            frontToFSensor.update();

            xSemaphoreGive(i2cMutex);
        }
        

        drive.update();

        vTaskDelay(pdMS_TO_TICKS(5)); 
    }
}