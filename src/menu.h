#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "icons.h"

// Declaraciones de funciones públicas del menú
void initMenu();
void handleMenuNavigation();
void displaySplashScreen();
void drawMenu();
void executeMenuItem();
void resetToSplash();

// Funciones para obtener el estado del menú
bool menuNeedsDisplayUpdate();
bool menuIsShowingSplash();
bool menuIsInMenu();
bool menuIsInSubMenu();
void setMenuDisplayUpdateFlag(bool flag);

// Función para inicializar el display del menú
void setMenuDisplay(Adafruit_SSD1306* displayPtr);

#endif