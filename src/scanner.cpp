// #include "scanner.h"
// #include <SPI.h>
// #include "menu.h"
// #include "config.h"

// static SPIClass spiNRF(VSPI);
// static RF24 radio(NRF1_CE, NRF1_CSN);
// static uint8_t channelActivity[CHANNELS];
// static uint8_t selectedChannel = 0;
// static ScannerState scannerState = SCANNER_IDLE;
// static bool scanningPaused = false;

// void initScanner() {
//     spiNRF.begin(NRF1_SCK, NRF1_MISO, NRF1_MOSI, NRF1_CSN);
//     radio.begin(&spiNRF);
//     radio.setPALevel(RF24_PA_MAX);
//     radio.setDataRate(RF24_1MBPS);
//     radio.setAutoAck(false);
//     radio.setRetries(0, 0);
//     radio.setPayloadSize(0);
//     radio.openReadingPipe(1, 0xE7E7E7E7E7LL);
//     radio.stopListening();
//     scannerState = SCANNER_IDLE;
//     Serial.println(radio.isChipConnected() ? "NRF conectado" : "NRF no detectado");
// }

// void runScanner(U8G2& u8g2, bool& shouldExit) {
//     if (scannerState == SCANNER_IDLE) {
//         memset(channelActivity, 0, sizeof(channelActivity));
//         scannerState = SCANNER_SCANNING;
//         selectedChannel = 0;
//     }

//     if (scannerState == SCANNER_SCANNING) {
//         for (uint8_t ch = 0; ch < CHANNELS; ++ch) {
//             uint16_t count = 0;
//             for (int s = 0; s < SAMPLES_PER_CHANNEL; ++s) {
//                 radio.setChannel(ch);
//                 radio.startListening();
//                 delayMicroseconds(130);
//                 if (radio.testRPD()) {
//                     count++;
//                 }
//                 radio.stopListening();
//                 delayMicroseconds(10);
                
//                 // Comprobar si se debe salir durante el escaneo
//                 ButtonState btn = readButtons();
//                 if (btn == BTN_LEFT_PRESSED || btn == BTN_RIGHT_PRESSED) {
//                     shouldExit = true;
//                     scannerState = SCANNER_IDLE;
//                     return;
//                 }
//             }
//             channelActivity[ch] = map(count, 0, SAMPLES_PER_CHANNEL, 0, 58);
//         }
//         scannerState = SCANNER_VISUALIZING;
//     }

//     if (scannerState == SCANNER_VISUALIZING) {
//         u8g2.clearBuffer();
//         u8g2.setFontMode(1);
//         u8g2.setBitmapMode(1);
//         u8g2.setFont(u8g2_font_4x6_tr);

//         char header[22];
//         snprintf(header, sizeof(header), "[CH: %3d  Lvl: %3d]", selectedChannel, channelActivity[selectedChannel]);
//         u8g2.drawStr(27, 6, header);

//         // Ejes verticales
//         u8g2.drawLine(2, 63, 2, 1);
//         u8g2.drawLine(124, 63, 124, 1);

//         // Ticks Y
//         for (int y = 10; y <= 52; y += 6) {
//             u8g2.drawLine(0, y, 7, y);
//             u8g2.drawLine(120, y, 127, y);
//         }

//         // Línea base eje X
//         u8g2.drawLine(0, 63, 127, 63);

//         // Dibujar barras
//         const int barOffset = 8;
//         for (uint8_t ch = 0; ch < CHANNELS; ++ch) {  // Mostrar todos los canales
//             int height = channelActivity[ch];
//             int x = ch + barOffset;
//             int y = 63;

//             if (ch == selectedChannel) {
//                 u8g2.setDrawColor(2);
//             }

//             u8g2.drawLine(x, y, x, y - height);

//             if (ch == selectedChannel) {
//                 u8g2.setDrawColor(1);
//             }
//         }

//         u8g2.sendBuffer();

//         // Manejar navegación de canales
//         ButtonState btn = readButtons();
//         if (btn == BTN_UP_PRESSED && selectedChannel > 0) {
//             selectedChannel--;
//         } 
//         else if (btn == BTN_DOWN_PRESSED && selectedChannel < CHANNELS - 1) {
//             selectedChannel++;
//         }
//         else if (btn == BTN_LEFT_PRESSED || btn == BTN_RIGHT_PRESSED) {
//             shouldExit = true;
//             scannerState = SCANNER_IDLE;
//         }
//     }
// }

#include "scanner.h"
#include <SPI.h>
#include "menu.h"

static SPIClass spiNRF(VSPI);
static RF24 radio(NRF1_CE, NRF1_CSN);
static uint8_t channelActivity[CHANNELS];
static uint8_t selectedChannel = 0;
static ScannerState scannerState = SCANNER_IDLE;
static bool scanningPaused = false;

void initScanner() {
    spiNRF.begin(NRF1_SCK, NRF1_MISO, NRF1_MOSI, NRF1_CSN);
    radio.begin(&spiNRF);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_1MBPS);
    radio.setAutoAck(false);
    radio.setRetries(0, 0);
    radio.setPayloadSize(0);
    radio.openReadingPipe(1, 0xE7E7E7E7E7LL);
    radio.stopListening();
    scannerState = SCANNER_IDLE;
    scanningPaused = false;
    Serial.println(radio.isChipConnected() ? "NRF conectado" : "NRF no detectado");
}

void runScanner(U8G2& u8g2, bool& shouldExit) {
    ButtonState btn = readButtons();
    
    // Manejar botón SELECT para pausar/reanudar
    if (btn == BTN_SELECT_PRESSED) {
        if (scannerState == SCANNER_SCANNING) {
            scannerState = SCANNER_PAUSED;
            scanningPaused = true;
            Serial.println("Escaneo pausado");
        } 
        else if (scannerState == SCANNER_PAUSED) {
            scannerState = SCANNER_SCANNING;
            scanningPaused = false;
            Serial.println("Escaneo reanudado");
        }
    }
    
    // Salir con botones izquierda/derecha
    if (btn == BTN_LEFT_PRESSED || btn == BTN_RIGHT_PRESSED) {
        shouldExit = true;
        scannerState = SCANNER_IDLE;
        return;
    }

    if (scannerState == SCANNER_IDLE) {
        memset(channelActivity, 0, sizeof(channelActivity));
        scannerState = SCANNER_SCANNING;
        selectedChannel = 0;
        scanningPaused = false;
    }

    if (scannerState == SCANNER_SCANNING && !scanningPaused) {
        for (uint8_t ch = 0; ch < CHANNELS; ++ch) {
            // Si está pausado, salir del bucle
            if (scanningPaused) break;
            
            uint16_t count = 0;
            for (int s = 0; s < SAMPLES_PER_CHANNEL; ++s) {
                radio.setChannel(ch);
                radio.startListening();
                delayMicroseconds(130);
                if (radio.testRPD()) {
                    count++;
                }
                radio.stopListening();
                delayMicroseconds(10);
                
                // Comprobar si se debe salir durante el escaneo
                ButtonState btn = readButtons();
                if (btn == BTN_LEFT_PRESSED || btn == BTN_RIGHT_PRESSED) {
                    shouldExit = true;
                    scannerState = SCANNER_IDLE;
                    return;
                }
            }
            channelActivity[ch] = map(count, 0, SAMPLES_PER_CHANNEL, 0, 58);
        }
        if (!scanningPaused) {
            scannerState = SCANNER_VISUALIZING;
        }
    }

    if (scannerState == SCANNER_VISUALIZING || scannerState == SCANNER_PAUSED) {
        u8g2.clearBuffer();
        u8g2.setFontMode(1);
        u8g2.setBitmapMode(1);
        u8g2.setFont(u8g2_font_4x6_tr);

        // Mostrar estado de pausa si está activo
        if (scanningPaused) {
            u8g2.drawStr(40, 6, "[PAUSED]");
        }
        
        char header[22];
        snprintf(header, sizeof(header), "[CH: %3d  Lvl: %3d]", selectedChannel, channelActivity[selectedChannel]);
        u8g2.drawStr(27, scanningPaused ? 16 : 6, header);

        // Ejes verticales
        u8g2.drawLine(2, 63, 2, 1);
        u8g2.drawLine(124, 63, 124, 1);

        // Ticks Y
        for (int y = 10; y <= 52; y += 6) {
            u8g2.drawLine(0, y, 7, y);
            u8g2.drawLine(120, y, 127, y);
        }

        // Línea base eje X
        u8g2.drawLine(0, 63, 127, 63);

        // Dibujar barras
        const int barOffset = 8;
        for (uint8_t ch = 0; ch < CHANNELS; ++ch) {
            int height = channelActivity[ch];
            int x = ch + barOffset;
            int y = 63;

            if (ch == selectedChannel) {
                u8g2.setDrawColor(2);
            }

            u8g2.drawLine(x, y, x, y - height);

            if (ch == selectedChannel) {
                u8g2.setDrawColor(1);
            }
        }

        u8g2.sendBuffer();

        // Manejar navegación de canales solo si no está pausado
        if (!scanningPaused) {
            ButtonState btn = readButtons();
            if (btn == BTN_UP_PRESSED && selectedChannel > 0) {
                selectedChannel--;
            } 
            else if (btn == BTN_DOWN_PRESSED && selectedChannel < CHANNELS - 1) {
                selectedChannel++;
            }
        }
    }
}