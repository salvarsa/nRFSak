#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "config.h"
#include "icons.h"
#include "menu.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ OLED_SCL, /* data=*/ OLED_SDA);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Iniciando nRFSak...");

    // Inicializar I2C para la pantalla OLED
    Wire.begin(OLED_SDA, OLED_SCL);

    u8g2.begin();
    u8g2.enableUTF8Print();

    setMenuDisplay(&u8g2);
    initMenu();
    displaySplashScreen();

    Serial.println("nRFSak iniciado correctamente!");
    Serial.println("Presiona LEFT o RIGHT para entrar al menu");
}

void loop() {
    handleMenuNavigation();

    if (menuNeedsDisplayUpdate()) {
        if (menuIsShowingSplash()) {
            displaySplashScreen();
        } else if (menuIsInMenu() && !menuIsInSubMenu()) {
            drawMenu();
        }
        setMenuDisplayUpdateFlag(false);
    }

    delay(10);
    yield();
}
