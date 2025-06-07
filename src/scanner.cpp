#include "scanner.h"
#include "config.h"
#include <Adafruit_SSD1306.h>

int channel[CHANNELS];
bool in_scanner_mode = false;
unsigned long lastScanTime = 0;
uint8_t sensorArray[128] = {0};  // Historial de lecturas para gráfico
extern Adafruit_SSD1306* display;
static bool spi_initialized = false;

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
  delayMicroseconds(10);
  SPI.transfer((reg & 0x1F) | 0x20); // Comando de escritura
  SPI.transfer(value);
  digitalWrite(NRF1_CSN, HIGH);
  delayMicroseconds(10);
}

void powerUp() {
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x02);
  delayMicroseconds(150);
}

void setRX() {
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x01);
  digitalWrite(NRF1_CE, HIGH);
  delayMicroseconds(130);
}

void disable() {
  digitalWrite(NRF1_CE, LOW);
  delayMicroseconds(10);
}

// FUNCIONES DE ESCANEO 

void scanChannels() {
  disable();
  memset(channel, 0, sizeof(channel));

  Serial.println("Iniciando escaneo de canales...");

  for (int ch = 0; ch < CHANNELS; ch++) {
    // Configurar canal (0-125)
    int frequency = (125 * ch) / CHANNELS;
    setRegister(_NRF24_RF_CH, frequency);
    
    int detections = 0;
    for (int s = 0; s < SAMPLES_PER_CHANNEL; s++) {
      setRX();
      delayMicroseconds(100);
      disable();
      
      // Verificar detección de portadora
      if (getRegister(_NRF24_RPD)) {
        detections++;
      }
      delayMicroseconds(10);
    }
    
    // Convertir a porcentaje
    channel[ch] = (detections * 100) / SAMPLES_PER_CHANNEL;
    
    // Log para debug cada 10 canales
    if (ch % 10 == 0) {
      Serial.print("Canal ");
      Serial.print(ch);
      Serial.print(" (");
      Serial.print(frequency);
      Serial.print("): ");
      Serial.print(channel[ch]);
      Serial.println("%");
    }
  }
  
  Serial.println("Escaneo completado");
}

void updateGraph() {
  if (!display) {
    Serial.println("Error: Display no disponible");
    return;
  }
  
  display->clearDisplay();
  
  // Encontrar valor máximo para normalización
  int maxVal = 0;
  int activeChannels = 0;
  for (int i = 0; i < CHANNELS; i++) {
    if (channel[i] > maxVal) maxVal = channel[i];
    if (channel[i] > 5) activeChannels++;
  }

  for (int i = 127; i > 0; i--) {
    sensorArray[i] = sensorArray[i-1];
  }
  sensorArray[0] = maxVal;
  
  // Dibujar gráfico
  for (int i = 0; i < CHANNELS && i < 64; i++) { // Limitar a 64 para que quepa en pantalla
    int height = map(channel[i], 0, 100, 0, 16);
    int x = i * 2; // 2 píxeles por canal
    if (height > 0) {
      display->drawLine(x, 31, x, 31 - height, SSD1306_WHITE);
      display->drawLine(x + 1, 31, x + 1, 31 - height, SSD1306_WHITE);
    }
  }
  
  // Información textual
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  
  // Título
  display->setCursor(0, 0);
  display->print("2.4GHz Scanner");
  
  // Información de estado
  display->setCursor(0, 8);
  display->print("Max: ");
  display->print(maxVal);
  display->print("% Active: ");
  display->print(activeChannels);

  // Indicador de actividad
  static bool blink = false;
  blink = !blink;
  if (blink) {
    display->setCursor(120, 0);
    display->print("*");
  }
  
  display->setCursor(0, 24);
  display->print("Press any key to exit");
  
  display->display();
}

// INTERFAZ PÚBLICA 

bool scannerSetup(Adafruit_SSD1306* displayPtr) {
  Serial.println("=== Iniciando modo escáner ===");
  
  if (!displayPtr) {
    Serial.println("Error: Display pointer es NULL");
    return false;
  }
  
  display = displayPtr;
  
  // Configurar pines
  pinMode(NRF1_CE, OUTPUT);
  pinMode(NRF1_CSN, OUTPUT);
  digitalWrite(NRF1_CE, LOW);
  digitalWrite(NRF1_CSN, HIGH);
  
  Serial.println("Pines configurados");
  
  // Inicializar SPI solo si no está inicializado
  if (!spi_initialized) {
    SPI.begin(NRF1_SCK, NRF1_MISO, NRF1_MOSI);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    spi_initialized = true;
    Serial.println("SPI inicializado");
  }
  
  delay(100); // Tiempo para estabilizar
  
  // Verificar comunicación con NRF24
  Serial.println("Verificando comunicación con NRF24...");
  
  // Test básico de comunicación
  setRegister(_NRF24_CONFIG, 0x08); // Valor de prueba
  delay(10);
  byte testVal = getRegister(_NRF24_CONFIG);
  
  Serial.print("Valor de prueba CONFIG: 0x");
  Serial.println(testVal, HEX);
  
  // Configurar NRF24
  Serial.println("Configurando NRF24...");
  powerUp();
  setRegister(_NRF24_EN_AA, 0x00);      // Deshabilitar Auto-Ack
  setRegister(_NRF24_RF_SETUP, 0x0F);   // Máxima potencia (0dBm)
  
  // Verificar configuración
  byte config = getRegister(_NRF24_CONFIG);
  byte rf_setup = getRegister(_NRF24_RF_SETUP);
  
  Serial.print("CONFIG final: 0x");
  Serial.println(config, HEX);
  Serial.print("RF_SETUP final: 0x");
  Serial.println(rf_setup, HEX);
  
  // Inicializar arrays
  memset(sensorArray, 0, sizeof(sensorArray));
  memset(channel, 0, sizeof(channel));
  
  in_scanner_mode = true;
  lastScanTime = 0; // Forzar primer escaneo inmediato
  
  Serial.println("=== Escáner listo! ===");
  
  // Mostrar pantalla inicial del scanner
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);
  display->println("Scanner Iniciado");
  display->println("Escaneando...");
  display->println("");
  display->println("Presiona cualquier");
  display->println("tecla para salir");
  display->display();
  
  return true;
}

void scannerLoop() {
  if (!in_scanner_mode || !display) {
    return;
  }
  
  if (millis() - lastScanTime > SCAN_INTERVAL) {
    Serial.print("Ejecutando escaneo... ");
    Serial.print("Tiempo transcurrido: ");
    Serial.println(millis() - lastScanTime);
    
    scanChannels();
    updateGraph();
    lastScanTime = millis();
  }
}

void scannerExit() {
  Serial.println("=== Saliendo del modo escáner ===");
  in_scanner_mode = false;
  disable(); // Asegurar que NRF24 esté apagado
}

bool isInScannerMode() {
  return in_scanner_mode;
}