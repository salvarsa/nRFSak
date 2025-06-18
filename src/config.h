#ifndef CONFIG_H
#define CONFIG_H

// Pines de la pantalla OLED (I2C)
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1

// Pines de los botones
#define BTN_UP 26
#define BTN_DOWN 33
#define BTN_LEFT 25
#define BTN_RIGHT 27
#define BTN_SELECT 32

// Pines para NRF24L01 - Antena 1 (Principal)
#define NRF1_CE 4
#define NRF1_CSN 2
#define NRF1_MOSI 23
#define NRF1_MISO 19
#define NRF1_SCK 18

// Pines para NRF24L01 - Antena 2
#define NRF2_CE 14
#define NRF2_CSN 12

// Pines para NRF24L01 - Antena 3
#define NRF3_CE 15
#define NRF3_CSN 13

// Configuraciones del menú
#define MENU_ITEMS 6
#define DEBOUNCE_DELAY 50

// Configuraciones del Scanner
#define CHANNELS 128    // nRF24L01 soporta canales 0-127 (2.4-2.527 GHz)
#define SAMPLES_PER_CHANNEL 100
#define SCAN_INTERVAL 100
#define CHANNEL_WIDTH 1   // 1 MHz por canal
#define BASE_FREQ 2400    // Frecuencia base en MHz

// Estados de los botones
enum ButtonState {
  BTN_NONE,
  BTN_UP_PRESSED,
  BTN_DOWN_PRESSED,
  BTN_LEFT_PRESSED,
  BTN_RIGHT_PRESSED,
  BTN_SELECT_PRESSED
};

// Elementos del menú principal
enum MenuItems {
  MENU_SCANNER = 0,
  MENU_ANALYZER = 1,
  MENU_JAMMER = 2,
  MENU_BLE_SPOOFER = 3,
  MENU_SOUR_APPLE = 4,
  MENU_SETTINGS = 5
};

// Estados del Scanner
enum ScannerState {
  SCANNER_IDLE,
  SCANNER_SCANNING,
  SCANNER_PAUSED,
  SCANNER_COMPLETE
};

#endif