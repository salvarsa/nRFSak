#include "scanner.h"
#include <SPI.h>
#include "menu.h"
#include <EEPROM.h>

static SPIClass spiNRF(VSPI);
static RF24 radio(NRF1_CE, NRF1_CSN);
static uint8_t sensorArray[128]; 
static uint8_t channelActivity[CHANNELS];  // Actividad por canal
static uint8_t selectedChannel = 0;
static ScannerState scannerState = SCANNER_IDLE;
static bool scanningPaused = false;
static unsigned long lastSaveTime = 0;
static const unsigned long saveInterval = 5000;
static const int EEPROM_ADDRESS_SENSOR_ARRAY = 0;
static uint8_t currentScanChannel = 0;
static uint16_t scanSampleCounter = 0;
static uint16_t scanHitCounter = 0;
static unsigned long lastScanTime = 0;
static const unsigned long SCAN_DELAY = 50;  // ms entre muestras

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
    currentScanChannel = 0;
    scanSampleCounter = 0;
    scanHitCounter = 0;
    
    EEPROM.begin(512);
    for (int i = 0; i < 128; i++) {
        sensorArray[i] = EEPROM.read(EEPROM_ADDRESS_SENSOR_ARRAY + i);
    }
    
    memset(channelActivity, 0, sizeof(channelActivity));
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
        scannerState = SCANNER_SCANNING;
        scanningPaused = false;
    }

    // Escaneo secuencial de canales
    if (scannerState == SCANNER_SCANNING && !scanningPaused) {
        if (millis() - lastScanTime > SCAN_DELAY) {
            lastScanTime = millis();
            
            radio.setChannel(currentScanChannel);
            radio.startListening();
            delayMicroseconds(130);
            
            if (radio.testRPD()) {
                scanHitCounter++;
            }
            
            radio.stopListening();
            delayMicroseconds(10);
            
            scanSampleCounter++;
            
            // Cambiar de canal después de suficientes muestras
            if (scanSampleCounter >= SAMPLES_PER_CHANNEL) {
                channelActivity[currentScanChannel] = map(
                    scanHitCounter, 
                    0, 
                    SAMPLES_PER_CHANNEL, 
                    0, 
                    58
                );
                
                scanSampleCounter = 0;
                scanHitCounter = 0;
                currentScanChannel = (currentScanChannel + 1) % CHANNELS;
            }
        }
    }

    // Actualizar gráfico de desplazamiento para el canal seleccionado
    if (millis() - lastScanTime > 100) {  // Actualizar gráfico cada 100ms
        // Desplazar el array
        for (int i = 0; i < 127; i++) {
            sensorArray[i] = sensorArray[i+1];
        }
        
        // Agregar nuevo dato (actividad del canal seleccionado)
        sensorArray[127] = channelActivity[selectedChannel];
    }

    // Dibujar pantalla
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.setFont(u8g2_font_4x6_tr);

    // Marco del gráfico
    u8g2.drawLine(0, 0, 0, 63);
    u8g2.drawLine(127, 0, 127, 63);

    // Marcadores en el eje Y
    for (int y = 10; y <= 52; y += 10) {
        u8g2.drawLine(127, y, 122, y);
        u8g2.drawLine(0, y, 5, y);
    }

    // Marcadores en el eje X
    for (int x = 10; x < 127; x += 10) {
        u8g2.drawPixel(x, 0);
        u8g2.drawPixel(x, 63);
    }

    // Dibujar gráfico de desplazamiento
    for (int x = 0; x < 128; x++) {
        u8g2.drawLine(x, 63, x, 63 - sensorArray[x]);
    }
    
    // Mostrar información
    char info[30];
    snprintf(info, sizeof(info), "CH:%d/%d Lvl:%d", 
        selectedChannel, 
        CHANNELS - 1,
        channelActivity[selectedChannel]
    );
    u8g2.drawStr(20, 10, info);
    
    // Mostrar progreso de escaneo
    char progress[20];
    // snprintf(progress, sizeof(progress), "Scan:%d/%d", 
    //     currentScanChannel, 
    //     CHANNELS - 1
    // );
    
    u8g2.drawStr(20, 20, progress);
    
    // Indicador de estado
    if (scanningPaused) {
        u8g2.drawStr(50, 30, "PAUSED");
    }

    u8g2.sendBuffer();

    // Manejar navegación de canales
    if (!scanningPaused) {
        if (btn == BTN_UP_PRESSED && selectedChannel > 0) {
            selectedChannel--;
        } 
        else if (btn == BTN_DOWN_PRESSED && selectedChannel < CHANNELS - 1) {
            selectedChannel++;
        }
    }

    // Guardar en EEPROM periódicamente
    if (millis() - lastSaveTime > saveInterval) {
        saveGraphToEEPROM();
    }
}