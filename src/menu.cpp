#include "menu.h"
#include "scanner.h"

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

static Adafruit_SSD1306* display = nullptr;

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

void setMenuDisplay(Adafruit_SSD1306* displayPtr) {
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
    
    Serial.println("Menu inicializado correctamente!");
}

void resetToSplash() {
    inMenu = false;
    inSubMenu = false;
    showSplash = true;
    displayNeedsUpdate = true;
    Serial.println("Regresando a pantalla de inicio");
}

// Función para mostrar la pantalla de splash/inicio personalizada
void displaySplashScreen() {
    if (!display) return;
    
    display->clearDisplay();
    display->setTextColor(SSD1306_WHITE);
    display->setTextWrap(false);
    
    // Verificar si tienes las fuentes disponibles
    #ifdef HAVE_CUSTOM_FONTS
        // Si tienes las fuentes personalizadas, usar el diseño original
        display->setFont(&FreeSerifBold9pt7b);
        display->setCursor(52, 16);
        display->print("nRFSak");
        
        display->drawBitmap(14, 1, logo_nrfSak, 30, 30, SSD1306_WHITE);
        
        display->setFont(&Picopixel);
        display->setCursor(73, 25);
        display->print("V 1.0");
    #else
        // Dibujar logo principal
        display->drawBitmap(14, 1, logo_nrfsak, 30, 30, SSD1306_WHITE);
        
        // Título principal con fuente estándar
        display->setTextSize(2);  // Texto más grande para el título
        display->setCursor(52, 8);
        display->print("nRFSak");
        
        // Versión con fuente más pequeña
        display->setTextSize(.5);
        display->setCursor(73, 25);
        display->print("V 1.0");
    #endif
    
    display->display();
}

void drawMenu() {
    if (!display) return;
    
    display->clearDisplay();

    switch(currentMenuItem) {
        case MENU_SCANNER:
            display->drawBitmap(58, 2, icon_scanner, 15, 16, SSD1306_WHITE);
            break;
            
        case MENU_ANALYZER:
            display->drawBitmap(52, 5, icon_analyzer, 17, 16, SSD1306_WHITE);
            break;
            
        case MENU_JAMMER:
            display->drawBitmap(55, 3, icon_jammer, 14, 16, SSD1306_WHITE);
            break;
            
        case MENU_BLE_SPOOFER:
            display->drawBitmap(51, 4, icon_ble, 14, 16, SSD1306_WHITE);
            break;
            
        case MENU_SOUR_APPLE:
            display->drawBitmap(50, 1, icon_apple, 18, 18, SSD1306_WHITE);
            break;
            
        case MENU_SETTINGS:
            display->drawBitmap(53, 3, icon_settings, 16, 16, SSD1306_WHITE);
            break;
    }
    
    // Nombre del elemento del menú debajo del icono con texto más pequeño
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    
    // Calcular posición centrada del texto
    int textWidth = strlen(menuItems[currentMenuItem]) * 6;
    int textX = (OLED_WIDTH - textWidth) / 2;
    
    // Posición Y del texto más abajo para dar más separación del icono
    display->setCursor(textX, 25);
    display->println(menuItems[currentMenuItem]);
    
    // Mostrar navegación en esquina superior izquierda
    display->setTextSize(.5);
    display->setCursor(2, 2);
    display->print(currentMenuItem + 1);
    display->print("/");
    display->print(MENU_ITEMS);
    
    display->display();
}

void handleMenuNavigation() {
    ButtonState buttonPressed = readButtons();
    if (buttonPressed == BTN_NONE) {
        return;
    }

    if (inMenu && !inSubMenu) {
        if (millis() - lastMenuActivity > menuTimeout) {
            Serial.println("Timeout del menu - regresando a splash");
            resetToSplash();
            return;
        }
    }
    
    if (!inMenu && !inSubMenu) {

        if (buttonPressed == BTN_LEFT_PRESSED || buttonPressed == BTN_RIGHT_PRESSED) {
            Serial.println("Entrando al menu principal");
            inMenu = true;
            showSplash = false;
            displayNeedsUpdate = true;
            lastMenuActivity = millis();
        }
    }
    else if (inMenu && !inSubMenu) {

        lastMenuActivity = millis();
        
        switch(buttonPressed) {
            case BTN_UP_PRESSED:
                Serial.print("Menu UP - Item actual: ");
                Serial.print(currentMenuItem);
                currentMenuItem--;
                if (currentMenuItem < 0) {
                    currentMenuItem = MENU_ITEMS - 1;
                }
                Serial.print(" -> Nuevo item: ");
                Serial.print(currentMenuItem);
                Serial.print(" (");
                Serial.print(menuItems[currentMenuItem]);
                Serial.println(")");
                displayNeedsUpdate = true;
                break;
                
            case BTN_DOWN_PRESSED:
                Serial.print("Menu DOWN - Item actual: ");
                Serial.print(currentMenuItem);
                currentMenuItem++;
                if (currentMenuItem >= MENU_ITEMS) {
                    currentMenuItem = 0;
                }
                Serial.print(" -> Nuevo item: ");
                Serial.print(currentMenuItem);
                Serial.print(" (");
                Serial.print(menuItems[currentMenuItem]);
                Serial.println(")");
                displayNeedsUpdate = true;
                break;
                
            case BTN_LEFT_PRESSED:
                Serial.println("Menu LEFT - saliendo del menu");
                resetToSplash();
                break;
                
            case BTN_RIGHT_PRESSED:
                Serial.println("Menu RIGHT - saliendo del menu");
                resetToSplash();
                break;
                
            case BTN_SELECT_PRESSED:
                Serial.print("Menu SELECT - ejecutando: ");
                Serial.println(menuItems[currentMenuItem]);
                inSubMenu = true;
                executeMenuItem();
                break;
                
            default:
                break;
        }
    }
    else if (inSubMenu) {
        Serial.println("Saliendo del submenu - regresando al menu");
        inSubMenu = false;
        displayNeedsUpdate = true;
        lastMenuActivity = millis();
    }
}

void executeMenuItem() {
    if (!display) return;
    
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    
    switch(currentMenuItem) {
        case MENU_SCANNER:
            display->println("Scanner Mode");
            display->println("Scanning 2.4GHz...");
            display->println("Press any key to exit");
            display->display();
            
            // Iniciar el modo escáner
            scannerSetup();
            in_scanner_mode = true;
            return;
            
        case MENU_ANALYZER:
            display->println("Analyzer Mode");
            display->println("Analyzing signals...");
            display->println("");
            display->println("Feature coming soon!");
            break;
            
        case MENU_JAMMER:
            display->println("Jammer Mode");
            display->println("WARNING: Use legally!");
            display->println("");
            display->println("Feature coming soon!");
            break;
            
        case MENU_BLE_SPOOFER:
            display->println("BLE Spoofer Mode");
            display->println("Spoofing BLE...");
            display->println("");
            display->println("Feature coming soon!");
            break;
            
        case MENU_SOUR_APPLE:
            display->println("Sour Apple Mode");
            display->println("Apple attack...");
            display->println("");
            display->println("Feature coming soon!");
            break;
            
        case MENU_SETTINGS:
            display->println("Settings");
            display->println("Configure device...");
            display->println("");
            display->println("Feature coming soon!");
            break;
    }
    
    display->println("");
    display->println("Press any key to return");
    display->display();
}

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