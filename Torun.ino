#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Button.h"
#include "Menu.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1);

Button btnUp(5);
Button btnSelect(18);

Menu menu(
    btnUp,
    btnSelect,
    display);

void setup()
{
    Wire.begin();

    display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C);

    display.clearDisplay();
    display.display();

    btnUp.begin();
    btnSelect.begin();

    menu.begin();
}

void loop()
{
    btnUp.update();
    btnSelect.update();

    menu.update();
}