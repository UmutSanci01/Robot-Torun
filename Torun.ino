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

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define VL53_ADDR 0x29
#define QMC5883_ADDR 0x2C

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
void testVL53();

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
        for(;;);
    }

    if (!imu.calibrate())
    {
        for(;;);
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

void loop()
{
    btnUp.update();
    btnSelect.update();

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

void ControlTask(void *pvParameters) {
    for(;;) {
        leftEncoder.update();
        rightEncoder.update();

        if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            imu.update();
            frontToFSensor.update();
            xSemaphoreGive(i2cMutex);
        }

        drive.update();

        // testVL53();
        // testManyetometreKonum();

        vTaskDelay(pdMS_TO_TICKS(5)); 
    }
}

void testManyetometreKonum() {
    static bool initialized = false;

    if (!initialized) {
        // Set/Reset periyodu
        Wire.beginTransmission(QMC5883_ADDR);
        Wire.write(0x0B);
        Wire.write(0x01);
        Wire.endTransmission();

        // Sensörü uyandır ve sürekli okuma moduna al
        Wire.beginTransmission(QMC5883_ADDR);
        Wire.write(0x0A);
        Wire.write(0x1D); 
        Wire.endTransmission();
        
        initialized = true;
        return; 
    }

    // Status register okuması (Yeni veri hazır mı?)
    Wire.beginTransmission(QMC5883_ADDR);
    Wire.write(0x06);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)QMC5883_ADDR, (uint8_t)1);
    
    if (Wire.available()) {
        uint8_t status = Wire.read();
        
        if (status & 0x01) { 
            Wire.beginTransmission(QMC5883_ADDR);
            Wire.write(0x00);
            Wire.endTransmission(false);
            Wire.requestFrom((uint8_t)QMC5883_ADDR, (uint8_t)6);
            
            if (Wire.available() >= 6) {
                int16_t x = Wire.read() | (Wire.read() << 8);
                int16_t y = Wire.read() | (Wire.read() << 8);
                int16_t z = Wire.read() | (Wire.read() << 8);
                
                // EMI (Elektromanyetik Parazit) filtresi
                static float fX = 0, fY = 0, fZ = 0;
                if (fX == 0 && fY == 0) { fX = x; fY = y; fZ = z; }
                
                fX = (x * 0.15f) + (fX * 0.85f);
                fY = (y * 0.15f) + (fY * 0.85f);
                fZ = (z * 0.15f) + (fZ * 0.85f);

                Serial.print("Manyetik_X:");
                Serial.print(fX);
                Serial.print("\tManyetik_Y:");
                Serial.print(fY);
                Serial.print("\tManyetik_Z:");
                Serial.println(fZ);
            }
        }
    }
}

void testVL53() {
    static bool initialized = false;
    static uint32_t lastRead = 0;

    if (!initialized) {
        // Cihaz kimliğini (Model ID) doğrula (0xC0 register'ı 0xEE döndürmelidir)
        Wire.beginTransmission(VL53_ADDR);
        Wire.write(0xC0);
        if (Wire.endTransmission(false) == 0) {
            Wire.requestFrom((uint8_t)VL53_ADDR, (uint8_t)1);
            if (Wire.available()) {
                uint8_t id = Wire.read();
                if (id == 0xEE) {
                    // Sürekli ölçüm modunu başlat (0x00 register'ına 0x02 yazılır)
                    Wire.beginTransmission(VL53_ADDR);
                    Wire.write(0x00);
                    Wire.write(0x02);
                    Wire.endTransmission();
                    initialized = true;
                }
            }
        } else {
            return;
        }
    }

    // 50 ms aralıklarla mesafe verisini oku ve Serial Plotter için bas
    if (millis() - lastRead >= 50) {
        lastRead = millis();

        // Sonuç register'ını işaretle (0x1E: Mesafe MSB)
        Wire.beginTransmission(VL53_ADDR);
        Wire.write(0x1E);
        Wire.endTransmission(false);

        Wire.requestFrom((uint8_t)VL53_ADDR, (uint8_t)2);
        if (Wire.available() >= 2) {
            uint16_t dist = (Wire.read() << 8) | Wire.read();

            // Serial Plotter formatı: Etiket:Deger
            Serial.print("Mesafe_mm:");
            Serial.println(dist);
        }
    }
}