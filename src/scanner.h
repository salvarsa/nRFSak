#ifndef SCANNER_H
#define SCANNER_H

#include <Arduino.h>
#include <SPI.h>
#include "config.h"
#include "menu.h" 

// Registros del NRF24L01+
#define _NRF24_CONFIG      0x00
#define _NRF24_EN_AA       0x01
#define _NRF24_RF_CH       0x05
#define _NRF24_RF_SETUP    0x06
#define _NRF24_RPD         0x09

// Configuración de escaneo
#define CHANNELS  64
#define SAMPLES_PER_CHANNEL 50
#define SCAN_INTERVAL 250 

bool scannerSetup(Adafruit_SSD1306* displayPtr);
void scannerSetup();
void scannerLoop();
void scannerExit();

extern bool in_scanner_mode;

#endif