#include "Button.h"
#include "Menu.h"

Button btnUp(5);
Button btnSelect(18);

Menu menu(btnUp, btnSelect);

void setup()
{
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