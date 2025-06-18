// #include "scanner.h"
// #include <SPI.h>
// #include "menu.h"

// static SPIClass spiNRF(VSPI);
// static RF24 radio(NRF1_CE, NRF1_CSN);
// static uint8_t channelActivity[CHANNELS];
// static uint8_t selectedChannel = 0;

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
//     Serial.println(radio.isChipConnected() ? "NRF conectado" : "NRF no detectado");
// }

// void runScanner(U8G2& u8g2) {
//     memset(channelActivity, 0, sizeof(channelActivity));

//     for (uint8_t ch = 0; ch < CHANNELS; ++ch) {
//         uint16_t count = 0;
//         for (int s = 0; s < SAMPLES_PER_CHANNEL; ++s) {
//             radio.setChannel(ch);
//             radio.startListening();
//             delayMicroseconds(130);
//             if (radio.testRPD()) {
//                 count++;
//             }
//             radio.stopListening();
//             delayMicroseconds(10);
//         }
//         channelActivity[ch] = map(count, 0, SAMPLES_PER_CHANNEL, 0, OLED_HEIGHT - 20);
//         Serial.printf("CH %3d: %3d hits => bar %d\n", ch, count, channelActivity[ch]);

//     }

//     radio.setChannel(40);  // Wi-Fi canal 1 (2412 MHz)
//     radio.startListening();
//     delayMicroseconds(150);
//     Serial.println(radio.testRPD() ? "Señal detectada en canal 40" : "Silencio en canal 40");
//     radio.stopListening();

//     // Dibujar gráfico
//     u8g2.clearBuffer();
//     u8g2.setFont(u8g2_font_5x7_tr);
//     u8g2.drawStr(2, 10, "Scanner 2.4 GHz");
//     // u8g2.setFont(u8g2_font_t0_11_tr);
//     // u8g2.drawStr((OLED_WIDTH - u8g2.getStrWidth("Scanner 2.4 GHz")) / 2, 10, "Scanner 2.4 GHz");

//     // Ejes Y
//     for (int y = 0; y <= OLED_HEIGHT - 20; y += 10) {
//         u8g2.drawHLine(0, OLED_HEIGHT - 1 - y, 4);
//         char val[3];
//         snprintf(val, sizeof(val), "%d", y);
//         u8g2.drawStr(5, OLED_HEIGHT - 1 - y, val);
//     }

//     // Eje X
//     for (uint8_t ch = 0; ch < OLED_WIDTH && ch < CHANNELS; ch += 10) {
//         char label[4];
//         snprintf(label, sizeof(label), "%d", ch);
//         u8g2.drawStr(ch, OLED_HEIGHT - 1, label);
//     }

//      // Dibujar barras
//         for (uint8_t ch = 0; ch < CHANNELS && ch < OLED_WIDTH; ++ch) {
//             int height = channelActivity[ch];
//             u8g2.drawBox(ch, OLED_HEIGHT - height - 1, 1, height);
//         }

//         // Barra destacada
//         u8g2.setDrawColor(2);
//         u8g2.drawBox(selectedChannel, OLED_HEIGHT - channelActivity[selectedChannel] - 2, 1, channelActivity[selectedChannel]);
//         u8g2.setDrawColor(1);

//         // Flecha + etiqueta
//         u8g2.drawTriangle(selectedChannel, 11, selectedChannel - 2, 15, selectedChannel + 2, 15);

//         char info[32];
//         snprintf(info, sizeof(info), "CH: %d  Lvl: %d", selectedChannel, channelActivity[selectedChannel]);
//         u8g2.drawStr(OLED_WIDTH - 64, 10, info);

//         u8g2.sendBuffer();
//         delay(300);

//         // Control navegación canal o salir
//         ButtonState btn = readButtons();
//         if (btn == BTN_LEFT_PRESSED && selectedChannel > 0) selectedChannel--;
//         if (btn == BTN_RIGHT_PRESSED && selectedChannel < CHANNELS - 1) selectedChannel++;
//         if (btn == BTN_SELECT_PRESSED);
// }


// File: src/scanner.cpp
#include "scanner.h"
#include <SPI.h>
#include "menu.h"

static SPIClass spiNRF(VSPI);
static RF24 radio(NRF1_CE, NRF1_CSN);
static uint8_t channelActivity[CHANNELS];
static uint8_t selectedChannel = 0;

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
    Serial.println(radio.isChipConnected() ? "NRF conectado" : "NRF no detectado");
}

void runScanner(U8G2& u8g2) {
    memset(channelActivity, 0, sizeof(channelActivity));

    for (uint8_t ch = 0; ch < CHANNELS; ++ch) {
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
        }
        channelActivity[ch] = map(count, 0, SAMPLES_PER_CHANNEL, 0, 58);
        Serial.printf("CH %3d: %3d hits => bar %d\n", ch, count, channelActivity[ch]);
    }

    // --- Visualización estilo nRFBox ---
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.setFont(u8g2_font_4x6_tr);

    char header[22];
    snprintf(header, sizeof(header), "[CH: %3d  Lvl: %3d]", selectedChannel, channelActivity[selectedChannel]);
    u8g2.drawStr(27, 6, header);

    // Ejes verticales
    u8g2.drawLine(2, 63, 2, 1);
    u8g2.drawLine(124, 63, 124, 1);

    // Ticks Y
    for (int y = 10; y <= 52; y += 6) {
        u8g2.drawLine(0, y, 7, y);
        u8g2.drawLine(120, y, 127, y);
    }

    // Línea base eje X
    u8g2.drawLine(0, 59, 127, 59);

    // Dibujar barras estilo línea vertical por canal
    const int barOffset = 8;
    const int barMaxHeight = 58;
    for (uint8_t ch = 0; ch < 90; ++ch) {
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
    delay(300);

    // Control navegación canal o salir
    ButtonState btn = readButtons();
    if (btn == BTN_LEFT_PRESSED && selectedChannel > 0) selectedChannel--;
    if (btn == BTN_RIGHT_PRESSED && selectedChannel < CHANNELS - 1) selectedChannel++;
    if (btn == BTN_SELECT_PRESSED) return;
}
