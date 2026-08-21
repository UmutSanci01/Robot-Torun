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

Motor leftMotor(26, 25, 14, 0);
Motor rightMotor(32, 33, 14, 1);
Encoder leftEncoder(34, 35);
Encoder rightEncoder(39, 36);


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
    imu
);



TaskHandle_t ControlTaskHandle;

void ControlTask(void *pvParameters) {
    for(;;) {
        leftEncoder.update();
        rightEncoder.update();

        if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            imu.update();
            xSemaphoreGive(i2cMutex);
        }

        drive.update();

        vTaskDelay(pdMS_TO_TICKS(5)); 
    }
}



void setup()
{
    Serial.begin(115200);

    Wire.begin();
    Wire.setClock(400000);

    i2cMutex = xSemaphoreCreateMutex();

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

    leftEncoder.setWheelDiameter(0.044f);
    rightEncoder.setWheelDiameter(0.044f);

    if (!imu.begin())
    {
        for(;;);
    }

    if (!imu.calibrate())
    {
        for(;;);
    }
    
    drive.begin();
    drive.disable();
    drive.stop();
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

    menu.begin();
}

void loop()
{
    btnUp.update();
    btnSelect.update();

    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        menu.update();
        xSemaphoreGive(i2cMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}