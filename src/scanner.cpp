#include "scanner.h"
#include "config.h"
#include <Adafruit_SSD1306.h>

int channel[CHANNELS];
bool in_scanner_mode = false;
unsigned long lastScanTime = 0;
uint8_t sensorArray[128] = {0};
static Adafruit_SSD1306* display = nullptr;
static SPIClass* nrf_spi = nullptr;
static bool nrf_initialized = false;

// FUNCIONES DEL NRF24 

byte getRegister(byte reg) {
  byte value;
  digitalWrite(NRF1_CSN, LOW);
  delayMicroseconds(10);
  value = nrf_spi->transfer(reg & 0x1F); // Comando de lectura
  value = nrf_spi->transfer(0x00);       // Leer valor
  digitalWrite(NRF1_CSN, HIGH);
  delayMicroseconds(10);
  return value;
}

void setRegister(byte reg, byte value) {
  digitalWrite(NRF1_CSN, LOW);
  delayMicroseconds(10);
  nrf_spi->transfer((reg & 0x1F) | 0x20); // Comando de escritura
  nrf_spi->transfer(value);
  digitalWrite(NRF1_CSN, HIGH);
  delayMicroseconds(10);
}

void powerUp() {
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x02);
  delay(5); // Tiempo de power-up más largo
}

void powerDown() {
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) & ~0x02);
  delay(1);
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

bool testNRF24Connection() {
  Serial.println("Probando conexión NRF24...");
  
  // Test 1: Escribir y leer registro CONFIG
  byte originalConfig = getRegister(_NRF24_CONFIG);
  Serial.print("CONFIG original: 0x");
  Serial.println(originalConfig, HEX);
  
  // Escribir valor de prueba
  setRegister(_NRF24_CONFIG, 0x08);
  delay(10);
  byte testRead = getRegister(_NRF24_CONFIG);
  Serial.print("CONFIG después de escribir 0x08: 0x");
  Serial.println(testRead, HEX);
  
  // Test 2: Probar registro RF_CH (debería aceptar valores 0-125)
  setRegister(_NRF24_RF_CH, 0x4C); // Canal 76
  delay(10);
  byte channelRead = getRegister(_NRF24_RF_CH);
  Serial.print("RF_CH después de escribir 0x4C: 0x");
  Serial.println(channelRead, HEX);
  
  // Restaurar valores
  setRegister(_NRF24_CONFIG, originalConfig);
  
  // Si podemos leer/escribir correctamente, la conexión está bien
  return (testRead == 0x08 && channelRead == 0x4C);
}

// FUNCIONES DE ESCANEO 

void scanChannels() {
  if (!nrf_initialized) {
    Serial.println("NRF24 no inicializado");
    return;
  }
  
  disable();
  memset(channel, 0, sizeof(channel));

  Serial.println("Iniciando escaneo de canales...");

  for (int ch = 0; ch < CHANNELS; ch++) {
    // Mapear canal a frecuencia (2400-2525 MHz)
    int frequency = ch * 2; // Cada canal = 2MHz
    if (frequency > 125) frequency = 125; // Límite máximo
    
    setRegister(_NRF24_RF_CH, frequency);
    delay(1); // Tiempo para que se establezca la frecuencia
    
    int detections = 0;
    for (int s = 0; s < SAMPLES_PER_CHANNEL; s++) {
      setRX();
      delayMicroseconds(200); // Tiempo de muestreo más largo
      
      // Leer RPD (Received Power Detector)
      if (getRegister(_NRF24_RPD) & 0x01) {
        detections++;
      }
      
      disable();
      delayMicroseconds(50);
      
      // Alimentar al watchdog
      if (s % 10 == 0) {
        yield();
      }
    }
    
    // Convertir a porcentaje
    channel[ch] = (detections * 100) / SAMPLES_PER_CHANNEL;
    
    // Log para debug cada 16 canales
    if (ch % 16 == 0) {
      Serial.print("Canal ");
      Serial.print(ch);
      Serial.print(" (");
      Serial.print(2400 + frequency);
      Serial.print("MHz): ");
      Serial.print(channel[ch]);
      Serial.println("%");
    }
    
    // Alimentar al watchdog
    if (ch % 8 == 0) {
      yield();
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

  // Actualizar array de historial
  for (int i = 127; i > 0; i--) {
    sensorArray[i] = sensorArray[i-1];
  }
  sensorArray[0] = maxVal;
  
  // Dibujar gráfico de barras
  int barWidth = 128 / CHANNELS;
  if (barWidth < 1) barWidth = 1;
  
  for (int i = 0; i < CHANNELS && i < 128/barWidth; i++) {
    int height = map(channel[i], 0, 100, 0, 20);
    int x = i * barWidth;
    
    if (height > 0) {
      for (int w = 0; w < barWidth - 1; w++) {
        display->drawLine(x + w, 31, x + w, 31 - height, SSD1306_WHITE);
      }
    }
  }
  
  // Información textual
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  
  // Título
  display->setCursor(0, 0);
  display->print("2.4GHz Scanner");
  
  // Indicador de actividad
  static bool blink = false;
  blink = !blink;
  if (blink) {
    display->setCursor(115, 0);
    display->print("*");
  }
  
  // Información de estado
  display->setCursor(0, 8);
  display->print("Max:");
  display->print(maxVal);
  display->print("% Act:");
  display->print(activeChannels);
  
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
  
  // Crear instancia SPI personalizada para NRF24
  if (nrf_spi == nullptr) {
    nrf_spi = new SPIClass(HSPI);
    nrf_spi->begin(NRF1_SCK, NRF1_MISO, NRF1_MOSI, NRF1_CSN);
    nrf_spi->setDataMode(SPI_MODE0);
    nrf_spi->setBitOrder(MSBFIRST);
    nrf_spi->setClockDivider(SPI_CLOCK_DIV4);
    Serial.println("SPI personalizado inicializado");
  }
  
  delay(100); // Tiempo para estabilizar
  
  // Verificar comunicación con NRF24
  if (!testNRF24Connection()) {
    Serial.println("ERROR: No se puede comunicar con NRF24L01+");
    Serial.println("Verifica las conexiones:");
    Serial.println("- VCC: 3.3V");
    Serial.println("- GND: GND");
    Serial.println("- CE: Pin 4");
    Serial.println("- CSN: Pin 2");
    Serial.println("- SCK: Pin 18");
    Serial.println("- MOSI: Pin 23");
    Serial.println("- MISO: Pin 19");
    
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->println("Scanner ERROR");
    display->println("NRF24 no detectado");
    display->println("");
    display->println("Verifica conexiones");
    display->println("Presiona para salir");
    display->display();
    
    return false;
  }
  
  Serial.println("NRF24L01+ detectado correctamente");
  
  // Configurar NRF24
  Serial.println("Configurando NRF24...");
  powerUp();
  setRegister(_NRF24_EN_AA, 0x00);      // Deshabilitar Auto-Ack
  setRegister(_NRF24_RF_SETUP, 0x0F);   // Máxima potencia y velocidad más baja
  setRegister(_NRF24_CONFIG, 0x0F);     // Power up, RX mode, CRC enabled
  
  // Verificar configuración
  byte config = getRegister(_NRF24_CONFIG);
  byte rf_setup = getRegister(_NRF24_RF_SETUP);
  
  Serial.print("CONFIG final: 0x");
  Serial.println(config, HEX);
  Serial.print("RF_SETUP final: 0x");
  Serial.println(rf_setup, HEX);
  
  nrf_initialized = true;
  
  // Inicializar arrays
  memset(sensorArray, 0, sizeof(sensorArray));
  memset(channel, 0, sizeof(channel));
  
  in_scanner_mode = true;
  lastScanTime = 0; // Forzar primer escaneo inmediato
  
  Serial.println("=== Escáner listo! ===");
  
  // Primera actualización de pantalla
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);
  display->println("Scanner Iniciado");
  display->println("Escaneando...");
  display->println("");
  display->println("Espera un momento...");
  display->display();
  
  return true;
}

void scannerLoop() {
  if (!in_scanner_mode || !display || !nrf_initialized) {
    return;
  }
  
  // Ejecutar escaneo según el intervalo
  if (millis() - lastScanTime > SCAN_INTERVAL) {
    Serial.println("Ejecutando escaneo...");
    
    scanChannels();
    updateGraph();
    lastScanTime = millis();
    
    Serial.println("Escaneo y gráfico actualizados");
  }
  
  // Pequeña pausa para evitar saturar el procesador
  delay(10);
}

void scannerExit() {
  Serial.println("=== Saliendo del modo escáner ===");
  in_scanner_mode = false;
  nrf_initialized = false;
  
  if (nrf_spi) {
    disable(); // Asegurar que NRF24 esté apagado
    powerDown(); // Apagar NRF24
  }
}

bool isInScannerMode() {
  return in_scanner_mode;
}