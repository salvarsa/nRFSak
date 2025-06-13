#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "icons.h"
#include "menu.h"


Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Iniciando nRFSak...");
    
    // Inicializar I2C para la pantalla OLED
    Wire.begin(OLED_SDA, OLED_SCL);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("Error: No se pudo inicializar la pantalla OLED");
        for(;;); // Loop infinito si no se puede inicializar la pantalla
    }
    
    setMenuDisplay(&display);
    initMenu();
    displaySplashScreen();
    
    Serial.println("nRFSak iniciado correctamente!");
    Serial.println("Presiona LEFT o RIGHT para entrar al menu");
}

void loop() {
    // Manejar navegación del menú principal
    handleMenuNavigation();
        
    // Actualizar pantalla si es necesario
        if (menuNeedsDisplayUpdate()) {
            if (menuIsShowingSplash()) {
                displaySplashScreen();
            } 
            else if (menuIsInMenu() && !menuIsInSubMenu()) {
                drawMenu();
            }
            setMenuDisplayUpdateFlag(false);
        }


    // Pequeña pausa para evitar consumo excesivo de CPU
    delay(10);
    
    // Alimentar al watchdog
    yield();
}