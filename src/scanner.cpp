#include "scanner.h"
#include "config.h"
#include <Adafruit_SSD1306.h>

int channel[CHANNELS];
bool in_scanner_mode = false;
unsigned long lastScanTime = 0;
uint8_t sensorArray[128] = {0};  // Historial de lecturas para gráfico

extern Adafruit_SSD1306* display;

// FUNCIONES DEL NRF24 

byte getRegister(byte reg) {
  byte value;
  digitalWrite(NRF1_CSN, LOW);
  SPI.transfer(reg & 0x1F); // Comando de lectura
  value = SPI.transfer(0);   // Leer valor
  digitalWrite(NRF1_CSN, HIGH);
  return value;
}

void setRegister(byte reg, byte value) {
  digitalWrite(NRF1_CSN, LOW);
  SPI.transfer((reg & 0x1F) | 0x20); // Comando de escritura
  SPI.transfer(value);
  digitalWrite(NRF1_CSN, HIGH);
}

void powerUp() {
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x02);
  delayMicroseconds(130);
}

void setRX() {
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x01);
  digitalWrite(NRF1_CE, HIGH);
  delayMicroseconds(100);
}

void disable() {
  digitalWrite(NRF1_CE, LOW);
}

// FUNCIONES DE ESCANEO 

void scanChannels() {
  disable();
  memset(channel, 0, sizeof(channel));  // Reiniciar valores

  for (int ch = 0; ch < CHANNELS; ch++) {
    // Configurar canal (0-125)
    setRegister(_NRF24_RF_CH, (125 * ch) / CHANNELS);
    
    for (int s = 0; s < SAMPLES_PER_CHANNEL; s++) {
      setRX();
      delayMicroseconds(100);
      disable();
      
      // Acumular detecciones
      if (getRegister(_NRF24_RPD)) {
        channel[ch]++;
      }
    }
    
    channel[ch] = (channel[ch] * 100) / SAMPLES_PER_CHANNEL;
  }
}

void updateGraph() {
  if (!display) return;
  
  display->clearDisplay();
  
  // Encontrar valor máximo para normalización
  int maxVal = 0;
  for (int i = 0; i < CHANNELS; i++) {
    if (channel[i] > maxVal) maxVal = channel[i];
  }

  for (int i = 127; i > 0; i--) {
    sensorArray[i] = sensorArray[i-1];
  }
  sensorArray[0] = maxVal;
  
  // Dibujar gráfico
  for (int x = 0; x < 128; x++) {
    int height = map(sensorArray[x], 0, 100, 0, 32);
    display->drawLine(x, 32, x, 32 - height, SSD1306_WHITE);
  }
  
  // Información textual
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  
  // Título
  display->setCursor(0, 0);
  display->print("2.4GHz Scanner");
  
  // Valor máximo actual
  display->setCursor(80, 0);
  display->print("Max:");
  display->print(maxVal);
  display->print("%");
  
  // Leyenda de canales
  display->setCursor(0, 10);
  display->print("W1");
  
  display->setCursor(30, 10);
  display->print("W6");
  
  display->setCursor(60, 10);
  display->print("W11");
  
  display->setCursor(90, 10);
  display->print("B/ZW");
  
  display->display();
}

// INTERFAZ PÚBLICA 

void scannerSetup() {
  Serial.println("Iniciando modo escáner...");
  
  // Configurar pines SPI
  pinMode(NRF1_CE, OUTPUT);
  pinMode(NRF1_CSN, OUTPUT);
  disable();
  
  // Inicializar SPI
  SPI.begin(NRF1_SCK, NRF1_MISO, NRF1_MOSI);
  SPI.setDataMode(SPI_MODE0);
  
  // Configurar NRF24
  powerUp();
  setRegister(_NRF24_EN_AA, 0x00);      // Deshabilitar Auto-Ack
  setRegister(_NRF24_RF_SETUP, 0x0F);   // Máxima potencia (0dBm)
  
  // Inicializar array de sensores
  memset(sensorArray, 0, sizeof(sensorArray));
  
  in_scanner_mode = true;
  Serial.println("Escáner listo!");
}

void scannerLoop() {
  if (millis() - lastScanTime > SCAN_INTERVAL) {
    scanChannels();
    updateGraph();
    lastScanTime = millis();
  }
}

void scannerExit() {
  in_scanner_mode = false;
  Serial.println("Saliendo del modo escáner");
}