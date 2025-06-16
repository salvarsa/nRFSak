// // menu.cpp
// #include "menu.h"
// #include "icons.h"
// #include <U8g2lib.h>

// static int currentMenuItem = 0;
// static bool inMenu = false;
// static bool inSubMenu = false;
// static bool displayNeedsUpdate = true;
// static bool showSplash = true;

// static unsigned long menuTimeout = 120000;
// static unsigned long lastMenuActivity = 0;

// static unsigned long lastButtonPress = 0;
// static const unsigned long DEBOUNCE_TIME = 200;
// static ButtonState lastButtonState = BTN_NONE;

// static U8G2_SSD1306_128X64_NONAME_F_HW_I2C* u8g2 = nullptr;

// static const char* menuItems[] = {
//   "Scanner",
//   "Analyzer", 
//   "Jammer",
//   "BLE Spoofer",
//   "Sour Apple",
//   "Settings"
// };

// static const unsigned char* menuIcons[] = {
//   icon_scanner,
//   icon_analyzer,
//   icon_jammer,
//   icon_ble,
//   icon_apple,
//   icon_settings
// };

// static const uint8_t iconWidths[] = {15, 17, 14, 14, 18, 16};
// static const uint8_t iconHeights[] = {16, 16, 16, 16, 18, 16};

// void setMenuDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayPtr) {
//     u8g2 = displayPtr;
// }

// static void initButtons() {
//     pinMode(BTN_UP, INPUT_PULLUP);
//     pinMode(BTN_DOWN, INPUT_PULLUP);
//     pinMode(BTN_LEFT, INPUT_PULLUP);
//     pinMode(BTN_RIGHT, INPUT_PULLUP);
//     pinMode(BTN_SELECT, INPUT_PULLUP);
// }

// ButtonState readButtons() {
//     if (millis() - lastButtonPress < DEBOUNCE_TIME) return BTN_NONE;

//     ButtonState currentButton = BTN_NONE;

//     if (digitalRead(BTN_UP) == LOW) currentButton = BTN_UP_PRESSED;
//     else if (digitalRead(BTN_DOWN) == LOW) currentButton = BTN_DOWN_PRESSED;
//     else if (digitalRead(BTN_LEFT) == LOW) currentButton = BTN_LEFT_PRESSED;
//     else if (digitalRead(BTN_RIGHT) == LOW) currentButton = BTN_RIGHT_PRESSED;
//     else if (digitalRead(BTN_SELECT) == LOW) currentButton = BTN_SELECT_PRESSED;

//     if (currentButton != BTN_NONE && currentButton != lastButtonState) {
//         lastButtonPress = millis();
//         lastButtonState = currentButton;
//         Serial.print("Boton detectado: ");
//         Serial.println(currentButton);
//         return currentButton;
//     }

//     if (currentButton == BTN_NONE) lastButtonState = BTN_NONE;
//     return BTN_NONE;
// }

// void initMenu() {
//     initButtons();
//     currentMenuItem = 0;
//     inMenu = false;
//     inSubMenu = false;
//     displayNeedsUpdate = true;
//     showSplash = true;
//     lastMenuActivity = 0;
//     lastButtonPress = 0;
//     lastButtonState = BTN_NONE;
//     Serial.println("Menu inicializado correctamente!");
// }

// void resetToSplash() {
//     inMenu = false;
//     inSubMenu = false;
//     showSplash = true;
//     displayNeedsUpdate = true;
//     Serial.println("Regresando a pantalla de inicio");
// }

// void displaySplashScreen() {
//     if (!u8g2) return;

//     u8g2->clearBuffer();
//     u8g2->drawXBMP(40, 16, 48, 16, logo_nrfsak);
//     u8g2->setFont(u8g2_font_helvB10_tf);
//     u8g2->drawStr(24, 12, "nRFSak");
//     u8g2->setFont(u8g2_font_5x7_tf);
//     u8g2->drawStr(90, 30, "v1.0");
//     u8g2->sendBuffer();
// }

// void drawMenu() {
//     if (!u8g2) return;
//     u8g2->clearBuffer();

//     u8g2->setFont(u8g2_font_5x7_tf);
//     int textWidth = u8g2->getStrWidth(menuItems[currentMenuItem]);
//     int textX = (OLED_WIDTH - textWidth) / 2;
//     u8g2->drawStr(textX, 10, menuItems[currentMenuItem]);

//     const unsigned char* icon = menuIcons[currentMenuItem];
//     uint8_t w = iconWidths[currentMenuItem];
//     uint8_t h = iconHeights[currentMenuItem];
//     int iconX = (OLED_WIDTH - w) / 2;
//     int iconY = (OLED_HEIGHT - h) / 2 + 5;
//     u8g2->drawXBMP(iconX, iconY, w, h, icon);

//     char buffer[16];
//     snprintf(buffer, sizeof(buffer), "%d/%d", currentMenuItem + 1, MENU_ITEMS);
//     u8g2->drawStr(2, 64 - 2, buffer);

//     u8g2->sendBuffer();
// }

// void handleMenuNavigation() {
//     ButtonState buttonPressed = readButtons();
//     if (buttonPressed == BTN_NONE) return;

//     if (inMenu && !inSubMenu && millis() - lastMenuActivity > menuTimeout) {
//         Serial.println("Timeout del menu - regresando a splash");
//         resetToSplash();
//         return;
//     }

//     if (!inMenu && !inSubMenu) {
//         if (buttonPressed == BTN_LEFT_PRESSED || buttonPressed == BTN_RIGHT_PRESSED) {
//             Serial.println("Entrando al menu principal");
//             inMenu = true;
//             showSplash = false;
//             displayNeedsUpdate = true;
//             lastMenuActivity = millis();
//         }
//     } else if (inMenu && !inSubMenu) {
//         lastMenuActivity = millis();
//         switch(buttonPressed) {
//             case BTN_UP_PRESSED:
//                 currentMenuItem = (currentMenuItem - 1 + MENU_ITEMS) % MENU_ITEMS;
//                 displayNeedsUpdate = true;
//                 break;
//             case BTN_DOWN_PRESSED:
//                 currentMenuItem = (currentMenuItem + 1) % MENU_ITEMS;
//                 displayNeedsUpdate = true;
//                 break;
//             case BTN_LEFT_PRESSED:
//             case BTN_RIGHT_PRESSED:
//                 resetToSplash();
//                 break;
//             case BTN_SELECT_PRESSED:
//                 inSubMenu = true;
//                 executeMenuItem();
//                 break;
//             default: break;
//         }
//     } else if (inSubMenu) {
//         inSubMenu = false;
//         displayNeedsUpdate = true;
//         lastMenuActivity = millis();
//     }
// }

// void executeMenuItem() {
//     if (!u8g2) return;

//     u8g2->clearBuffer();
//     u8g2->setFont(u8g2_font_5x7_tf);
//     u8g2->drawStr(0, 10, menuItems[currentMenuItem]);
//     u8g2->drawStr(0, 25, "Feature coming soon!");
//     u8g2->drawStr(0, 40, "Press any key to return");
//     u8g2->sendBuffer();
// }

// bool menuNeedsDisplayUpdate() { return displayNeedsUpdate; }
// bool menuIsShowingSplash() { return showSplash; }
// bool menuIsInMenu() { return inMenu; }
// bool menuIsInSubMenu() { return inSubMenu; }
// void setMenuDisplayUpdateFlag(bool flag) { displayNeedsUpdate = flag; }



#include "menu.h"

// Variables del menú
static int currentMenuItem = 0;
static bool inMenu = false;
static bool inSubMenu = false;
static bool displayNeedsUpdate = true;
static bool showSplash = true;

static unsigned long menuTimeout = 120000;
static unsigned long lastMenuActivity = 0;

static unsigned long lastButtonPress = 0;
static const unsigned long DEBOUNCE_TIME = 200;
static ButtonState lastButtonState = BTN_NONE;

static U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display = nullptr;


static const char* menuItems[] = {
  "Scanner",
  "Analyzer", 
  "Jammer",
  "BLE Spoofer",
  "Sour Apple",
  "Settings"
};

static const unsigned char* menuIcons[] = {
  icon_scanner,
  icon_analyzer,
  icon_jammer,
  icon_ble,
  icon_apple,
  icon_settings
};

static void initButtons();
ButtonState readButtons();

void setMenuDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayPtr) {
    display = displayPtr;
}



static void initButtons() {
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);
}

ButtonState readButtons() {
    if (millis() - lastButtonPress < DEBOUNCE_TIME) {
        return BTN_NONE;
    }
    
    ButtonState currentButton = BTN_NONE;
    
    if (digitalRead(BTN_UP) == LOW) {
        currentButton = BTN_UP_PRESSED;
    }
    else if (digitalRead(BTN_DOWN) == LOW) {
        currentButton = BTN_DOWN_PRESSED;
    }
    else if (digitalRead(BTN_LEFT) == LOW) {
        currentButton = BTN_LEFT_PRESSED;
    }
    else if (digitalRead(BTN_RIGHT) == LOW) {
        currentButton = BTN_RIGHT_PRESSED;
    }
    else if (digitalRead(BTN_SELECT) == LOW) {
        currentButton = BTN_SELECT_PRESSED;
    }

    if (currentButton != BTN_NONE && currentButton != lastButtonState) {
        lastButtonPress = millis();
        lastButtonState = currentButton;
        Serial.print("Boton detectado: ");
        Serial.println(currentButton);
        return currentButton;
    }

    if (currentButton == BTN_NONE) {
        lastButtonState = BTN_NONE;
    }
    
    return BTN_NONE;
}

void initMenu() {
    initButtons();
    
    currentMenuItem = 0;
    inMenu = false;
    inSubMenu = false;
    displayNeedsUpdate = true;
    showSplash = true;
    lastMenuActivity = 0;
    lastButtonPress = 0;
    lastButtonState = BTN_NONE;
    
};

void resetToSplash() {
    inMenu = false;
    inSubMenu = false;
    showSplash = true;
    displayNeedsUpdate = true;
    Serial.println("Regresando a pantalla de inicio");
}

void displaySplashScreen() {
    if (!display) return;
    
    display->clearBuffer();
    
    // Dibujar logo principal (si está disponible)
    display->drawXBMP(14, 8, 48, 16, logo_nrfsak);
    
    // Título principal
    display->setFont(u8g2_font_helvB10_tf);
    display->drawStr(52, 25, "nRFSak");
    
    // Versión
    display->setFont(u8g2_font_6x10_tf);
    display->drawStr(75, 35, "V 1.0");
    
    display->sendBuffer();
}

void drawMenu() {
    if (!display) return;
    
    display->clearBuffer();

    // Dibujar icono del menú actual
    switch(currentMenuItem) {
        case MENU_SCANNER:
            display->drawXBMP(56, 8, 15, 16, icon_scanner);
            break;
        case MENU_ANALYZER:
            display->drawXBMP(54, 8, 17, 16, icon_analyzer);
            break;
        case MENU_JAMMER:
            display->drawXBMP(57, 8, 14, 16, icon_jammer);
            break;
        case MENU_BLE_SPOOFER:
            display->drawXBMP(57, 8, 14, 16, icon_ble);
            break;
        case MENU_SOUR_APPLE:
            display->drawXBMP(55, 8, 18, 18, icon_apple);
            break;
        case MENU_SETTINGS:
            display->drawXBMP(56, 8, 16, 16, icon_settings);
            break;
    }
    
    // Nombre del elemento del menú
    display->setFont(u8g2_font_helvB08_tf);
    int textWidth = display->getStrWidth(menuItems[currentMenuItem]);
    int textX = (OLED_WIDTH - textWidth) / 2;
    display->drawStr(textX, 38, menuItems[currentMenuItem]);
    
    // Mostrar navegación
    display->setFont(u8g2_font_5x7_tf);
    char navStr[10];
    snprintf(navStr, sizeof(navStr), "%d/%d", currentMenuItem + 1, MENU_ITEMS);
    display->drawStr(2, 10, navStr);
    
    
    display->sendBuffer();
}

void handleMenuNavigation() {
    ButtonState buttonPressed = readButtons();
    if (buttonPressed == BTN_NONE) return;

    if (inMenu && !inSubMenu && millis() - lastMenuActivity > menuTimeout) {
        Serial.println("Timeout del menu - regresando a splash");
        resetToSplash();
        return;
    }

    if (!inMenu && !inSubMenu) {
        if (buttonPressed == BTN_LEFT_PRESSED || buttonPressed == BTN_RIGHT_PRESSED) {
            Serial.println("Entrando al menu principal");
            inMenu = true;
            showSplash = false;
            displayNeedsUpdate = true;
            lastMenuActivity = millis();
        }
    } else if (inMenu && !inSubMenu) {
        lastMenuActivity = millis();
        switch(buttonPressed) {
            case BTN_UP_PRESSED:
                currentMenuItem = (currentMenuItem - 1 + MENU_ITEMS) % MENU_ITEMS;
                displayNeedsUpdate = true;
                break;
            case BTN_DOWN_PRESSED:
                currentMenuItem = (currentMenuItem + 1) % MENU_ITEMS;
                displayNeedsUpdate = true;
                break;
            case BTN_LEFT_PRESSED:
            case BTN_RIGHT_PRESSED:
                resetToSplash();
                break;
            case BTN_SELECT_PRESSED:
                inSubMenu = true;
                executeMenuItem();
                break;
            default: break;
        }
    } else if (inSubMenu) {
        inSubMenu = false;
        displayNeedsUpdate = true;
        lastMenuActivity = millis();
    }
}

void executeMenuItem() {
    if (!display) return;
    
    Serial.print("Ejecutando item del menú: ");
    Serial.println(menuItems[currentMenuItem]);
    
    switch(currentMenuItem) {
        case MENU_SCANNER:
            break;
            
        case MENU_ANALYZER:
        case MENU_JAMMER:
        case MENU_BLE_SPOOFER:
        case MENU_SOUR_APPLE:
        case MENU_SETTINGS:
            display->clearBuffer();
            display->setFont(u8g2_font_helvB08_tf);
            display->drawStr(0, 12, menuItems[currentMenuItem]);
            display->setFont(u8g2_font_6x10_tf);
            display->drawStr(0, 28, "Feature coming soon!");
            display->drawStr(0, 45, "Press LEFT/RIGHT");
            display->drawStr(0, 55, "to return");
            display->sendBuffer();
            break;
    }
}

// === FUNCIONES DE ESTADO DEL MENÚ ===

bool menuNeedsDisplayUpdate() {
    return displayNeedsUpdate;
}

bool menuIsShowingSplash() {
    return showSplash;
}

bool menuIsInMenu() {
    return inMenu;
}

bool menuIsInSubMenu() {
    return inSubMenu;
}

void setMenuDisplayUpdateFlag(bool flag) {
    displayNeedsUpdate = flag;
}