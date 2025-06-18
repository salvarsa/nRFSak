#include "scanner.h"
#include <SPI.h> 

static SPIClass spiNRF(VSPI);
static RF24 radio(NRF1_CE, NRF1_CSN);
static uint8_t channelActivity[CHANNELS];

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
        channelActivity[ch] = map(count, 0, SAMPLES_PER_CHANNEL, 0, OLED_HEIGHT - 20);
        Serial.printf("CH %3d: %3d hits => bar %d\n", ch, count, channelActivity[ch]);

    }

    radio.setChannel(40);  // Wi-Fi canal 1 (2412 MHz)
    radio.startListening();
    delayMicroseconds(150);
    Serial.println(radio.testRPD() ? "Señal detectada en canal 40" : "Silencio en canal 40");
    radio.stopListening();

    // Dibujar gráfico
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(2, 10, "Scanner 2.4 GHz");

    for (uint8_t ch = 0; ch < CHANNELS && ch < OLED_WIDTH; ++ch) {
        int height = channelActivity[ch];
        u8g2.drawBox(ch, OLED_HEIGHT - height - 1, 1, height);
    }

    u8g2.sendBuffer();
    delay(500);
}
