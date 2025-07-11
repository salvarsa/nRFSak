// #ifndef MENU_H
// #define MENU_H

// #pragma once
// #include <Arduino.h>
// #include <Wire.h>
// #include <U8g2lib.h>
// #include "config.h"
// #include "icons.h"

// // Funciones públicas del menú
// void initMenu();
// void handleMenuNavigation();
// void displaySplashScreen();
// void drawMenu();
// void executeMenuItem();
// void resetToSplash();
// ButtonState readButtons();

// // Estado del menú
// bool menuNeedsDisplayUpdate();
// bool menuIsShowingSplash();
// bool menuIsInMenu();
// bool menuIsInSubMenu();
// void setMenuDisplayUpdateFlag(bool flag);

// // Inicializar display del menú
// void setMenuDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayPtr);

// #endif

// ========================================
// FILE: src/menu.h - VERSIÓN ACTUALIZADA
// ========================================

#ifndef MENU_H
#define MENU_H

#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "config.h"
#include "icons.h"

// Funciones públicas del menú
void initMenu();
void handleMenuNavigation();
void displaySplashScreen();
void drawMenu();
void executeMenuItem();
void resetToSplash();
ButtonState readButtons();

// Estado del menú
bool menuNeedsDisplayUpdate();
bool menuIsShowingSplash();
bool menuIsInMenu();
bool menuIsInSubMenu();
void setMenuDisplayUpdateFlag(bool flag);

// Inicializar display del menú
void setMenuDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayPtr);

#endif