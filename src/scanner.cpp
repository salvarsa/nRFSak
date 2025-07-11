// #include "scanner.h"
// #include <SPI.h>
// #include "menu.h"
// #include <EEPROM.h>

// static SPIClass spiNRF(VSPI);
// static RF24 radio(NRF1_CE, NRF1_CSN);
// static uint8_t sensorArray[128]; 
// static uint8_t channelRaw[CHANNELS];
// static uint8_t activeChannel = 0;
// static ScannerState scannerState = SCANNER_IDLE;
// static bool scanningPaused = false;
// static unsigned long lastSaveTime = 0;
// static const unsigned long saveInterval = 5000;
// static const int EEPROM_ADDRESS_SENSOR_ARRAY = 0;

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
//     scanningPaused = false;
//     activeChannel = 0;

//     EEPROM.begin(512);
//     for (int i = 0; i < 128; i++) {
//         sensorArray[i] = EEPROM.read(EEPROM_ADDRESS_SENSOR_ARRAY + i);
//     }

//     memset(channelRaw, 0, sizeof(channelRaw));
//     Serial.println(radio.isChipConnected() ? "NRF conectado" : "NRF no detectado");
// }

// void saveGraphToEEPROM() {
//     for (int i = 0; i < 128; i++) {
//         EEPROM.write(EEPROM_ADDRESS_SENSOR_ARRAY + i, sensorArray[i]);
//     }
//     EEPROM.commit();
//     lastSaveTime = millis();
// }

// void runScanner(U8G2& u8g2, bool& shouldExit) {
//     ButtonState btn = readButtons();

//     if (btn == BTN_SELECT_PRESSED) {
//         scanningPaused = !scanningPaused;
//         scannerState = scanningPaused ? SCANNER_PAUSED : SCANNER_SCANNING;
//         Serial.println(scanningPaused ? "Escaneo pausado" : "Escaneo reanudado");
//     }

//     if (btn == BTN_LEFT_PRESSED || btn == BTN_RIGHT_PRESSED) {
//         shouldExit = true;
//         scannerState = SCANNER_IDLE;
//         return;
//     }

//     if (scannerState == SCANNER_IDLE) {
//         scannerState = SCANNER_SCANNING;
//         scanningPaused = false;
//     }

//     if (scannerState == SCANNER_SCANNING && !scanningPaused) {
//         uint8_t maxHits = 0;
//         for (uint8_t ch = 0; ch < CHANNELS; ch++) {
//             uint8_t hits = 0;
//             radio.setChannel(ch);
//             for (uint8_t s = 0; s < SAMPLES_PER_CHANNEL; s++) {
//                 radio.startListening();
//                 delayMicroseconds(110);
//                 if (radio.testRPD()) hits++;
//                 radio.stopListening();
//                 delayMicroseconds(10);
//             }
//             channelRaw[ch] = hits;
//             if (hits > maxHits) {
//                 maxHits = hits;
//                 activeChannel = ch;
//             }
//         }

//         // Actualización del gráfico con escala FIJA (SAMPLES_PER_CHANNEL)
//         for (int i = 0; i < 127; i++) {
//             sensorArray[i] = sensorArray[i+1];
//         }
//         sensorArray[127] = map(
//             channelRaw[activeChannel], 
//             0, 
//             SAMPLES_PER_CHANNEL, 
//             0, 
//             63
//         );
//     }

//     // Dibujado siempre (incluso cuando está en pausa)
//     u8g2.clearBuffer();
//     u8g2.setFontMode(1);
//     u8g2.setBitmapMode(1);
//     u8g2.setFont(u8g2_font_4x6_tr);

//     // Marco del gráfico
//     u8g2.drawLine(0, 0, 0, 63);
//     u8g2.drawLine(127, 0, 127, 63);
    
//     // Marcadores en Y
//     for (int y = 10; y <= 52; y += 10) {
//         u8g2.drawLine(127, y, 122, y);
//         u8g2.drawLine(0, y, 5, y);
//     }
    
//     // Marcadores en X
//     for (int x = 10; x < 127; x += 10) {
//         u8g2.drawPixel(x, 0);
//         u8g2.drawPixel(x, 63);
//     }
    
//     // Gráfico de desplazamiento (estilo nRFBox)
//     for (int x = 0; x < 128; x++) {
//         u8g2.drawLine(x, 63, x, 63 - sensorArray[x]);
//     }

//     // Información del canal
//     char info[30];
//     snprintf(info, sizeof(info), "CH:%d Lvl:%d", activeChannel, channelRaw[activeChannel]);
//     u8g2.drawStr(20, 10, info);

//     // Indicador de pausa
//     if (scanningPaused) {
//         u8g2.drawStr(50, 20, "PAUSED");
//     }

//     u8g2.sendBuffer();

//     // Guardar en EEPROM periódicamente
//     if (millis() - lastSaveTime > saveInterval) {
//         saveGraphToEEPROM();
//     }
// }

#include "scanner.h"
#include <SPI.h>
#include "menu.h"
#include <EEPROM.h>

static SPIClass spiNRF(VSPI);
static RF24 radio(NRF1_CE, NRF1_CSN);
static uint8_t sensorArray[128]; 
static uint8_t channelRaw[CHANNELS];
static uint8_t activeChannel = 0;
static ScannerState scannerState = SCANNER_IDLE;
static bool scanningPaused = false;
static unsigned long lastSaveTime = 0;
static const unsigned long saveInterval = 5000;
static const int EEPROM_ADDRESS_SENSOR_ARRAY = 0;

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
    activeChannel = 0;

    EEPROM.begin(512);
    for (int i = 0; i < 128; i++) {
        sensorArray[i] = EEPROM.read(EEPROM_ADDRESS_SENSOR_ARRAY + i);
    }

    memset(channelRaw, 0, sizeof(channelRaw));
    Serial.println(radio.isChipConnected() ? "NRF conectado" : "NRF no detectado");
}

void saveGraphToEEPROM() {
    for (int i = 0; i < 128; i++) {
        EEPROM.write(EEPROM_ADDRESS_SENSOR_ARRAY + i, sensorArray[i]);
    }
    EEPROM.commit();
    lastSaveTime = millis();
}

void runScanner(U8G2& u8g2, bool& shouldExit) {
    ButtonState btn = readButtons();

    if (btn == BTN_SELECT_PRESSED) {
        scanningPaused = !scanningPaused;
        scannerState = scanningPaused ? SCANNER_PAUSED : SCANNER_SCANNING;
        Serial.println(scanningPaused ? "Escaneo pausado" : "Escaneo reanudado");
    }

    if (btn == BTN_LEFT_PRESSED || btn == BTN_RIGHT_PRESSED) {
        shouldExit = true;
        scannerState = SCANNER_IDLE;
        return;
    }

    if (scannerState == SCANNER_IDLE) {
        scannerState = SCANNER_SCANNING;
        scanningPaused = false;
    }

    if (scannerState == SCANNER_SCANNING && !scanningPaused) {
        uint8_t maxHits = 0;
        for (uint8_t ch = 0; ch < CHANNELS; ch++) {
            uint8_t hits = 0;
            radio.setChannel(ch);
            for (uint8_t s = 0; s < SAMPLES_PER_CHANNEL; s++) {
                radio.startListening();
                delayMicroseconds(110);
                if (radio.testRPD()) hits++;
                radio.stopListening();
                delayMicroseconds(10);
            }
            channelRaw[ch] = hits;
            if (hits > maxHits) {
                maxHits = hits;
                activeChannel = ch;
            }
        }

        for (int i = 0; i < 127; i++) {
            sensorArray[i] = sensorArray[i+1];
        }
        sensorArray[127] = map(channelRaw[activeChannel], 0, SAMPLES_PER_CHANNEL, 0, 55); // altura hasta y=8
    }

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    u8g2.drawLine(0, 63, 127, 63);
    u8g2.drawLine(0, 0, 0, 63);
    u8g2.drawLine(127, 0, 127, 63);

    for (int y = 58; y >= 8; y -= 5) {
        u8g2.drawLine(5, y, 1, y);
        u8g2.drawLine(126, y, 122, y);
    }

    u8g2.setFont(u8g2_font_4x6_tr);
    char label[20];
    snprintf(label, sizeof(label), "CH: %d", activeChannel);
    u8g2.drawStr(34, 7, label);

    for (int x = 0; x < 128; x++) {
        u8g2.drawLine(x, 63, x, 63 - sensorArray[x]);
    }

    if (scanningPaused) {
        u8g2.drawStr(50, 20, "PAUSED");
    }

    u8g2.sendBuffer();

    if (millis() - lastSaveTime > saveInterval) {
        saveGraphToEEPROM();
    }
}
