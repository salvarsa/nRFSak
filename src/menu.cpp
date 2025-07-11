// #include "menu.h"
// #include "icons.h"
// #include "scanner.h"

// // Variables del menú
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

// static U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display = nullptr;

// static const char* menuItems[] = {
//   "Scanner",
//   "Analizer", 
//   "Jammer",
//   "Spoofer",
//   "Sour apple",
//   "Information"
// };

// // Iconos correspondientes a cada menú (ya están definidos en icons.h)
// static const unsigned char* menuIcons[] = {
//   icon_scanner,
//   icon_analyzer,
//   icon_jammer,
//   icon_ble,
//   icon_apple,
//   icon_settings
// };

// static void initButtons();
// ButtonState readButtons();

// void setMenuDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayPtr) {
//     display = displayPtr;
// }

// static void initButtons() {
//     pinMode(BTN_UP, INPUT_PULLUP);
//     pinMode(BTN_DOWN, INPUT_PULLUP);
//     pinMode(BTN_LEFT, INPUT_PULLUP);
//     pinMode(BTN_RIGHT, INPUT_PULLUP);
//     pinMode(BTN_SELECT, INPUT_PULLUP);
// }

// ButtonState readButtons() {
//     if (millis() - lastButtonPress < DEBOUNCE_TIME) {
//         return BTN_NONE;
//     }
    
//     ButtonState currentButton = BTN_NONE;
    
//     if (digitalRead(BTN_UP) == LOW) {
//         currentButton = BTN_UP_PRESSED;
//     }
//     else if (digitalRead(BTN_DOWN) == LOW) {
//         currentButton = BTN_DOWN_PRESSED;
//     }
//     else if (digitalRead(BTN_LEFT) == LOW) {
//         currentButton = BTN_LEFT_PRESSED;
//     }
//     else if (digitalRead(BTN_RIGHT) == LOW) {
//         currentButton = BTN_RIGHT_PRESSED;
//     }
//     else if (digitalRead(BTN_SELECT) == LOW) {
//         currentButton = BTN_SELECT_PRESSED;
//     }

//     if (currentButton != BTN_NONE && currentButton != lastButtonState) {
//         lastButtonPress = millis();
//         lastButtonState = currentButton;
//         Serial.print("Boton detectado: ");
//         Serial.println(currentButton);
//         return currentButton;
//     }

//     if (currentButton == BTN_NONE) {
//         lastButtonState = BTN_NONE;
//     }
    
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
// }

// void resetToSplash() {
//     inMenu = false;
//     inSubMenu = false;
//     showSplash = true;
//     displayNeedsUpdate = true;
//     Serial.println("Regresando a pantalla de inicio");
// }

// void displaySplashScreen() {
//     if (!display) return;
    
//     display->clearBuffer();
//     display->setFontMode(1);
//     display->setBitmapMode(1);
    
//     // Marco principal
//     display->drawFrame(3, 3, 122, 57);
    
//     // Línea divisoria superior
//     display->drawLine(4, 14, 123, 14);
    
//     // Logo principal
//     display->drawXBMP(41, 18, 40, 40, logo_nrfsak);
    
//     // Título
//     display->setFont(u8g2_font_t0_11_tr);
//     display->drawStr(40, 13, "nRFSack");
    
//     // Versión
//     display->setFont(u8g2_font_4x6_tr);
//     display->drawStr(7, 56, "V 1.0");
    
//     display->sendBuffer();
// }

// void drawMenu() {
//     if (!display) return;
    
//     display->clearBuffer();
//     display->setFontMode(1);
//     display->setBitmapMode(1);
    
//     // Marco principal
//     display->drawFrame(3, 3, 122, 57);
    
//     // Línea divisoria superior
//     display->drawLine(4, 14, 123, 14);
    
//     // Línea vertical separadora para el contador
//     display->drawLine(25, 4, 25, 14);
    
//     // Título del menú actual
//     display->setFont(u8g2_font_t0_11_tr);
//     display->drawStr(40, 13, menuItems[currentMenuItem]);
    
//     // Contador de posición (ejemplo: "1/6")
//     display->setDrawColor(2);
//     display->setFont(u8g2_font_5x7_tr);
//     char counterStr[10];
//     snprintf(counterStr, sizeof(counterStr), "%d/6", currentMenuItem + 1);
//     display->drawStr(8, 12, counterStr);
    
//     // Restablecer color de dibujo
//     display->setDrawColor(1);
    
//     // Dibujar icono específico según el menú actual
//     switch(currentMenuItem) {
//         case MENU_SCANNER:
//             display->drawXBMP(41, 17, 40, 40, icon_scanner);
//             break;
            
//         case MENU_ANALYZER:
//             display->drawXBMP(41, 17, 40, 40, icon_analyzer);
//             break;
            
//         case MENU_JAMMER:
//             // Línea adicional para el jammer
//             display->drawLine(49, 37, 70, 37);
//             display->drawXBMP(40, 17, 40, 40, icon_jammer);
//             break;
            
//         case MENU_BLE_SPOOFER:
//             display->drawXBMP(36, 12, 50, 50, icon_ble);
//             break;
            
//         case MENU_SOUR_APPLE:
//             // Marco ligeramente diferente para Sour Apple
//             display->drawXBMP(41, 17, 40, 40, icon_apple);
//             break;
            
//         case MENU_SETTINGS:
//             display->drawXBMP(42, 17, 40, 40, icon_settings);
//             break;
//     }
    
//     display->sendBuffer();
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
//         // Salir del submenú
//         if (buttonPressed == BTN_LEFT_PRESSED || buttonPressed == BTN_RIGHT_PRESSED) {
//             inSubMenu = false;
//             displayNeedsUpdate = true;
//             lastMenuActivity = millis();
//         }
//     }
// }

// void executeMenuItem() {
//     if (!display) return;
    
//     Serial.print("Ejecutando item del menú: ");
//     Serial.println(menuItems[currentMenuItem]);
    
//     switch(currentMenuItem) {
//     case MENU_SCANNER:
//     {
//         bool shouldExitScanner = false;
//         initScanner();
//         while (!shouldExitScanner && menuIsInSubMenu()) {
//             runScanner(*display, shouldExitScanner);
            
//             // Salir si se detecta botón de salida
//             if (shouldExitScanner) {
//                 inSubMenu = false;
//                 setMenuDisplayUpdateFlag(true);
//                 break;
//             }
            
//             delay(10);
//         }
//         break;
//     }

//         case MENU_ANALYZER:
//             display->clearBuffer();
//             display->setFont(u8g2_font_helvB08_tf);
//             display->drawStr(0, 12, "Analyzer");
//             display->setFont(u8g2_font_6x10_tf);
//             display->drawStr(0, 28, "Feature coming soon!");
//             display->drawStr(0, 45, "Press LEFT/RIGHT");
//             display->drawStr(0, 55, "to return");
//             display->sendBuffer();
//             break;
            
//         case MENU_JAMMER:
//             display->clearBuffer();
//             display->setFont(u8g2_font_helvB08_tf);
//             display->drawStr(0, 12, "Jammer");
//             display->setFont(u8g2_font_6x10_tf);
//             display->drawStr(0, 28, "Feature coming soon!");
//             display->drawStr(0, 45, "Press LEFT/RIGHT");
//             display->drawStr(0, 55, "to return");
//             display->sendBuffer();
//             break;
            
//         case MENU_BLE_SPOOFER:
//             display->clearBuffer();
//             display->setFont(u8g2_font_helvB08_tf);
//             display->drawStr(0, 12, "BLE Spoofer");
//             display->setFont(u8g2_font_6x10_tf);
//             display->drawStr(0, 28, "Feature coming soon!");
//             display->drawStr(0, 45, "Press LEFT/RIGHT");
//             display->drawStr(0, 55, "to return");
//             display->sendBuffer();
//             break;
            
//         case MENU_SOUR_APPLE:
//             display->clearBuffer();
//             display->setFont(u8g2_font_helvB08_tf);
//             display->drawStr(0, 12, "Sour Apple");
//             display->setFont(u8g2_font_6x10_tf);
//             display->drawStr(0, 28, "Feature coming soon!");
//             display->drawStr(0, 45, "Press LEFT/RIGHT");
//             display->drawStr(0, 55, "to return");
//             display->sendBuffer();
//             break;
            
//         case MENU_SETTINGS:
//             display->clearBuffer();
//             display->setFont(u8g2_font_helvB08_tf);
//             display->drawStr(0, 12, "Information");
//             display->setFont(u8g2_font_6x10_tf);
//             display->drawStr(0, 28, "nRFSak v1.0");
//             display->drawStr(0, 38, "2.4GHz Toolkit");
//             display->drawStr(0, 55, "Press LEFT/RIGHT");
//             display->sendBuffer();
//             break;
//     }
// }

// // === FUNCIONES DE ESTADO DEL MENÚ ===

// bool menuNeedsDisplayUpdate() {
//     return displayNeedsUpdate;
// }

// bool menuIsShowingSplash() {
//     return showSplash;
// }

// bool menuIsInMenu() {
//     return inMenu;
// }

// bool menuIsInSubMenu() {
//     return inSubMenu;
// }

// void setMenuDisplayUpdateFlag(bool flag) {
//     displayNeedsUpdate = flag;
// }

#include "menu.h"
#include "icons.h"
#include "scanner.h"

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
  "Analizer", 
  "Jammer",
  "Spoofer",
  "Sour apple",
  "Information"
};

// Iconos correspondientes a cada menú (ya están definidos en icons.h)
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
}

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
    display->setFontMode(1);
    display->setBitmapMode(1);
    
    // Marco principal con esquinas redondeadas
    display->drawRFrame(2, 2, 123, 59, 5);
    
    // Título principal
    display->setFont(u8g2_font_t0_11_tr);
    display->drawStr(44, 13, "nRFSak");
    
    // Línea divisoria horizontal
    display->drawLine(2, 14, 124, 14);
    
    // Línea divisoria vertical izquierda
    display->drawLine(30, 2, 30, 14);
    
    // Área de versión con fondo
    display->drawXBMP(3, 3, 28, 11, image_Layer_8_bits);
    
    // Texto de versión (color invertido)
    display->setDrawColor(2);
    display->setFont(u8g2_font_4x6_tr);
    display->drawStr(8, 11, "V 1.0");
    
    // Restablecer color de dibujo
    display->setDrawColor(1);
    
    // Subtítulo
    display->drawStr(16, 58, "Hacker swiss army knife");
    
    // Logo principal (usando el logo existente del archivo icons.h)
    display->drawXBMP(42, 15, 39, 39, logo_nrfsak);
    
    // Flecha izquierda
    display->drawXBMP(11, 33, 7, 5, image_arrow_left_bits);
    
    // Flecha derecha
    display->drawXBMP(109, 33, 7, 5, image_arrow_right_bits);
    
    // Texto "menu" en ambos lados (color invertido)
    display->setDrawColor(2);
    display->drawStr(8, 44, "menu");
    display->drawStr(104, 44, "menu");
    
    // Restablecer color
    display->setDrawColor(1);
    
    display->sendBuffer();
}

void drawMenu() {
    if (!display) return;
    
    display->clearBuffer();
    display->setFontMode(1);
    display->setBitmapMode(1);
    
    // Marco principal con esquinas redondeadas
    display->drawRFrame(2, 2, 123, 59, 5);
    
    // Línea divisoria superior
    display->drawLine(2, 14, 124, 14);
    
    // Línea vertical separadora para el contador
    display->drawLine(30, 2, 30, 14);
    
    // Área del contador con fondo
    display->drawXBMP(3, 3, 28, 11, image_Layer_8_bits);
    
    // Contador de posición (ejemplo: "1/6") - color invertido
    display->setDrawColor(2);
    display->setFont(u8g2_font_4x6_tr);
    char counterStr[10];
    snprintf(counterStr, sizeof(counterStr), "%d/6", currentMenuItem + 1);
    display->drawStr(11, 11, counterStr);
    
    // Restablecer color de dibujo
    display->setDrawColor(1);
    
    // Título del menú actual
    display->setFont(u8g2_font_t0_11_tr);
    display->drawStr(43, 13, menuItems[currentMenuItem]);
    
    // Texto "press select" en la parte inferior
    display->setFont(u8g2_font_4x6_tr);
    display->drawStr(37, 58, "press select");
    
    // Flechas de navegación izquierda y derecha
    display->drawXBMP(5, 17, 7, 5, image_arrow_left_bits);
    display->drawXBMP(115, 17, 7, 5, image_arrow_right_bits);
    
    // Texto "exit" en color invertido
    display->setDrawColor(2);
    display->drawStr(14, 22, "exit");
    display->drawStr(98, 22, "exit");
    
    // Restablecer color
    display->setDrawColor(1);
    
    // Flechas up/down
    display->drawXBMP(107, 33, 5, 7, image_arrow_up_bits);
    display->drawXBMP(15, 33, 5, 7, image_arrow_down_bits);
    
    // Texto "menu" en color invertido
    display->setDrawColor(2);
    display->drawStr(10, 46, "menu");
    display->drawStr(102, 46, "menu");
    
    // Restablecer color
    display->setDrawColor(1);
    
    // Dibujar icono específico según el menú actual
    switch(currentMenuItem) {
        case MENU_SCANNER:
            display->drawXBMP(42, 17, 35, 35, icon_scanner);
            break;
            
        case MENU_ANALYZER:
            display->drawXBMP(38, 11, 45, 45, icon_analyzer);
            break;
            
        case MENU_JAMMER:
            display->drawXBMP(42, 17, 35, 35, icon_jammer);
            break;
            
        case MENU_BLE_SPOOFER:
            display->drawXBMP(42, 17, 35, 35, icon_ble);
            break;
            
        case MENU_SOUR_APPLE:
            display->drawXBMP(42, 17, 35, 35, icon_apple);
            break;
            
        case MENU_SETTINGS:
            display->drawXBMP(42, 17, 35, 35, icon_settings);
            break;
    }
    
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
        // Salir del submenú
        if (buttonPressed == BTN_LEFT_PRESSED || buttonPressed == BTN_RIGHT_PRESSED) {
            inSubMenu = false;
            displayNeedsUpdate = true;
            lastMenuActivity = millis();
        }
    }
}

void executeMenuItem() {
    if (!display) return;
    
    Serial.print("Ejecutando item del menú: ");
    Serial.println(menuItems[currentMenuItem]);
    
    switch(currentMenuItem) {
    case MENU_SCANNER:
    {
        bool shouldExitScanner = false;
        initScanner();
        while (!shouldExitScanner && menuIsInSubMenu()) {
            runScanner(*display, shouldExitScanner);
            
            // Salir si se detecta botón de salida
            if (shouldExitScanner) {
                inSubMenu = false;
                setMenuDisplayUpdateFlag(true);
                break;
            }
            
            delay(10);
        }
        break;
    }

        case MENU_ANALYZER:
            display->clearBuffer();
            display->setFont(u8g2_font_helvB08_tf);
            display->drawStr(0, 12, "Analyzer");
            display->setFont(u8g2_font_6x10_tf);
            display->drawStr(0, 28, "Feature coming soon!");
            display->drawStr(0, 45, "Press LEFT/RIGHT");
            display->drawStr(0, 55, "to return");
            display->sendBuffer();
            break;
            
        case MENU_JAMMER:
            display->clearBuffer();
            display->setFont(u8g2_font_helvB08_tf);
            display->drawStr(0, 12, "Jammer");
            display->setFont(u8g2_font_6x10_tf);
            display->drawStr(0, 28, "Feature coming soon!");
            display->drawStr(0, 45, "Press LEFT/RIGHT");
            display->drawStr(0, 55, "to return");
            display->sendBuffer();
            break;
            
        case MENU_BLE_SPOOFER:
            display->clearBuffer();
            display->setFont(u8g2_font_helvB08_tf);
            display->drawStr(0, 12, "BLE Spoofer");
            display->setFont(u8g2_font_6x10_tf);
            display->drawStr(0, 28, "Feature coming soon!");
            display->drawStr(0, 45, "Press LEFT/RIGHT");
            display->drawStr(0, 55, "to return");
            display->sendBuffer();
            break;
            
        case MENU_SOUR_APPLE:
            display->clearBuffer();
            display->setFont(u8g2_font_helvB08_tf);
            display->drawStr(0, 12, "Sour Apple");
            display->setFont(u8g2_font_6x10_tf);
            display->drawStr(0, 28, "Feature coming soon!");
            display->drawStr(0, 45, "Press LEFT/RIGHT");
            display->drawStr(0, 55, "to return");
            display->sendBuffer();
            break;
            
        case MENU_SETTINGS:
            display->clearBuffer();
            display->setFont(u8g2_font_helvB08_tf);
            display->drawStr(0, 12, "Information");
            display->setFont(u8g2_font_6x10_tf);
            display->drawStr(0, 28, "nRFSak v1.0");
            display->drawStr(0, 38, "2.4GHz Toolkit");
            display->drawStr(0, 55, "Press LEFT/RIGHT");
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