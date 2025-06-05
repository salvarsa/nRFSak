#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "icons.h"

// Inicialización de la pantalla OLED
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// Variables globales del menú
int currentMenuItem = 0;
bool inMenu = false;          // Empezamos fuera del menú
bool inSubMenu = false;       // Para controlar cuando estamos en una función
bool displayNeedsUpdate = true;
bool showSplash = true;       // Controlar pantalla de inicio

// Variables para timeout del menú (2 minutos = 120000 ms)
unsigned long menuTimeout = 120000;
unsigned long lastMenuActivity = 0;

// Variables mejoradas para debounce de botones
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_TIME = 200; // Aumentar tiempo de debounce a 200ms
ButtonState lastButtonState = BTN_NONE;

// Declaraciones de funciones (prototipos)
void initButtons();
ButtonState readButtons();
void displaySplashScreen();
void drawMenu();
void handleMenuNavigation();
void executeMenuItem();
void resetToSplash();

// Nombres de los elementos del menú
const char* menuItems[] = {
  "Scanner",
  "Analyzer", 
  "Jammer",
  "BLE Spoofer",
  "Sour Apple",
  "Settings"
};

// Iconos correspondientes a cada elemento del menú
const unsigned char* menuIcons[] = {
  icon_scanner,
  icon_analyzer,
  icon_jammer,
  icon_ble,
  icon_apple,
  icon_settings
};

// Función para inicializar los pines de los botones
void initButtons() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
}

// Función mejorada para leer el estado de los botones con debounce robusto
ButtonState readButtons() {
  // Si no ha pasado suficiente tiempo desde la última pulsación, no leer
  if (millis() - lastButtonPress < DEBOUNCE_TIME) {
    return BTN_NONE;
  }
  
  ButtonState currentButton = BTN_NONE;
  
  // Leer solo un botón a la vez, con prioridad
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
  
  // Solo procesar si hay un botón presionado Y es diferente al último
  if (currentButton != BTN_NONE && currentButton != lastButtonState) {
    lastButtonPress = millis();
    lastButtonState = currentButton;
    Serial.print("Boton detectado: ");
    Serial.println(currentButton);
    return currentButton;
  }
  
  // Si no hay botón presionado, resetear el estado
  if (currentButton == BTN_NONE) {
    lastButtonState = BTN_NONE;
  }
  
  return BTN_NONE;
}

// Función para resetear a la pantalla de splash
void resetToSplash() {
  inMenu = false;
  inSubMenu = false;
  showSplash = true;
  displayNeedsUpdate = true;
  Serial.println("Regresando a pantalla de inicio");
}

// Función para mostrar la pantalla de splash/inicio
void displaySplashScreen() {
  display.clearDisplay();
  
  // Mostrar logo principal
  display.drawBitmap(40, 0, logo_nrfbox, 48, 16, SSD1306_WHITE);
  
  // Texto de bienvenida
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 20);
  display.println("nRFSak");
  display.setCursor(5, 28);
  display.println("Press LEFT/RIGHT");
  
  display.display();
}

// Función para dibujar el menú principal
void drawMenu() {
  display.clearDisplay();
  
  // Dibujar icono centrado
  int iconX = (OLED_WIDTH - 16) / 2;  // Centrar icono de 16px
  int iconY = 2;
  display.drawBitmap(iconX, iconY, menuIcons[currentMenuItem], 16, 16, SSD1306_WHITE);
  
  // Nombre del elemento del menú debajo del icono
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Calcular posición centrada del texto
  int textWidth = strlen(menuItems[currentMenuItem]) * 6; // Aproximadamente 6px por carácter
  int textX = (OLED_WIDTH - textWidth) / 2;
  display.setCursor(textX, 22);
  display.println(menuItems[currentMenuItem]);
  
  // Mostrar navegación en esquina inferior derecha (texto más pequeño)
  display.setTextSize(1);
  display.setCursor(95, 25);
  display.print(currentMenuItem + 1);
  display.print("/");
  display.print(MENU_ITEMS);
  
  // Instrucciones de navegación en la parte inferior
  display.setTextSize(1);
  display.setCursor(0, 57);
  display.print("UP/DOWN:Nav SEL:Enter");
  
  display.display();
}

// Función para manejar la navegación del menú
void handleMenuNavigation() {
  ButtonState buttonPressed = readButtons();
  
  // Si no hay botón presionado, no hacer nada
  if (buttonPressed == BTN_NONE) {
    return;
  }
  
  // Verificar timeout del menú (solo cuando estamos en el menú, no en splash ni submenú)
  if (inMenu && !inSubMenu) {
    if (millis() - lastMenuActivity > menuTimeout) {
      Serial.println("Timeout del menu - regresando a splash");
      resetToSplash();
      return;
    }
  }
  
  if (!inMenu && !inSubMenu) {
    // Estamos en pantalla de inicio, solo LEFT o RIGHT entran al menú
    if (buttonPressed == BTN_LEFT_PRESSED || buttonPressed == BTN_RIGHT_PRESSED) {
      Serial.println("Entrando al menu principal");
      inMenu = true;
      showSplash = false;
      displayNeedsUpdate = true;
      lastMenuActivity = millis(); // Resetear timer de actividad
    }
  }
  else if (inMenu && !inSubMenu) {
    // Estamos en el menú principal - SOLO NAVEGACIÓN
    lastMenuActivity = millis(); // Actualizar actividad en cualquier botón
    
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
    // Estamos en un submenú, cualquier botón nos regresa al menú principal
    Serial.println("Saliendo del submenu - regresando al menu");
    inSubMenu = false;
    displayNeedsUpdate = true;
    lastMenuActivity = millis(); // Resetear timer al regresar al menú
  }
}

// Función para ejecutar la acción del elemento del menú seleccionado
void executeMenuItem() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  switch(currentMenuItem) {
    case MENU_SCANNER:
      display.println("Scanner Mode");
      display.println("Scanning 2.4GHz...");
      display.println("");
      display.println("Feature coming soon!");
      break;
      
    case MENU_ANALYZER:
      display.println("Analyzer Mode");
      display.println("Analyzing signals...");
      display.println("");
      display.println("Feature coming soon!");
      break;
      
    case MENU_JAMMER:
      display.println("Jammer Mode");
      display.println("WARNING: Use legally!");
      display.println("");
      display.println("Feature coming soon!");
      break;
      
    case MENU_BLE_SPOOFER:
      display.println("BLE Spoofer Mode");
      display.println("Spoofing BLE...");
      display.println("");
      display.println("Feature coming soon!");
      break;
      
    case MENU_SOUR_APPLE:
      display.println("Sour Apple Mode");
      display.println("Apple attack...");
      display.println("");
      display.println("Feature coming soon!");
      break;
      
    case MENU_SETTINGS:
      display.println("Settings");
      display.println("Configure device...");
      display.println("");
      display.println("Feature coming soon!");
      break;
  }
  
  display.println("");
  display.println("Press any key to return");
  display.display();
}

// Función de configuración inicial
void setup() {
  Serial.begin(115200);
  delay(1000); // Dar tiempo al Serial Monitor
  Serial.println("Iniciando nRFBox...");
  
  // Inicializar I2C para la pantalla OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // Inicializar pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error: No se pudo inicializar la pantalla OLED");
    for(;;); // Loop infinito si no se puede inicializar la pantalla
  }
  
  // Inicializar botones
  initButtons();
  
  // Mostrar pantalla de splash
  displaySplashScreen();
  
  Serial.println("nRFBox iniciado correctamente!");
  Serial.println("Presiona LEFT o RIGHT para entrar al menu");
}

// Loop principal
void loop() {
  // Manejar navegación siempre
  handleMenuNavigation();
  
  // Actualizar pantalla solo si es necesario
  if (displayNeedsUpdate) {
    if (showSplash) {
      displaySplashScreen();
    } else if (inMenu && !inSubMenu) {
      drawMenu(); // Solo mostrar el menú de navegación
    } 
    displayNeedsUpdate = false;
  }
  
  // Pequeña pausa para evitar consumo excesivo de CPU
  delay(50); // Aumenté el delay para dar más tiempo al debounce
}