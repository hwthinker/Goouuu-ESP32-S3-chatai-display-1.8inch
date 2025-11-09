/*
 * ESP32-S3 Complete Hardware Test
 * Board: ESP32-S3 N16R8 with 1.8" LCD (ST7735 128x160)
 * Tests: LCD Display + MAX98357A Speaker + INMP441 Microphone
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>  // <-- GANTI: gunakan ST7735
#include <driver/i2s.h>
#include <math.h>

// ========== LCD PINS (ST7735 1.8") ==========
#define PIN_TFT_SCLK 21  // SCL
#define PIN_TFT_MOSI 47  // SDA/MOSI
#define PIN_TFT_MISO -1  // not used
#define PIN_TFT_CS 41
#define PIN_TFT_DC 40
#define PIN_TFT_RST 45
#define PIN_TFT_BL 42  // Backlight

// Resolusi ST7735 1.8"
#define TFT_WIDTH 128
#define TFT_HEIGHT 160

// ========== SPEAKER PINS (MAX98357A) ==========
#define SPEAKER_DOUT 7   // DIN
#define SPEAKER_BCLK 15  // BCLK
#define SPEAKER_LRC 16   // LRC

// ========== MICROPHONE PINS (INMP441) ==========
#define MIC_WS 4   // Word Select
#define MIC_SCK 5  // Serial Clock
#define MIC_SD 6   // Serial Data

// ========== I2S CONFIGURATION ==========
#define SAMPLE_RATE 16000
#define I2S_SPEAKER I2S_NUM_0
#define I2S_MIC I2S_NUM_1

// ========== OBJECTS ==========
SPIClass spi(FSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

// ========== VARIABLES ==========
int32_t mic_buffer[512];
int16_t speaker_buffer[512];
bool speaker_playing = false;
double phase = 0.0;

// ========== LCD FUNCTIONS ==========
void initLCD() {
  // Start SPI
  spi.begin(PIN_TFT_SCLK, PIN_TFT_MISO, PIN_TFT_MOSI);

  // Turn on backlight
  pinMode(PIN_TFT_BL, OUTPUT);
  digitalWrite(PIN_TFT_BL, HIGH);

  // Inisialisasi ST7735
  // Coba BLACKTAB dulu; jika warna/offset aneh, ganti ke GREENTAB atau REDTAB.
  tft.initR(INITR_BLACKTAB);
  tft.setSPISpeed(20000000);  // 20 MHz (awal uji bisa turunkan jika perlu)
  tft.setRotation(2);         // samakan orientasi dengan versi ST7789 lama
  tft.fillScreen(ST77XX_BLACK);

  // Frame luar
  tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ST77XX_BLUE);
  tft.setTextWrap(false);

  // Header (pas untuk lebar 128)
  tft.setCursor(22, 6);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.print("ESP32-S3");

  tft.setCursor(12, 28);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.print("HW TEST");

  // Garis pemisah
  tft.drawLine(5, 45, 123, 45, ST77XX_WHITE);
}

void updateLCDStatus(const char* device, const char* status, uint16_t color) {
  static int line = 0;
  // Area status: Y=52..120, tiap baris 14 px
  int y = 52 + (line * 14);

  if (line >= 5) {
    // Clear status area (x=5..122, y=52..120)
    tft.fillRect(5, 52, 118, 70, ST77XX_BLACK);
    line = 0;
    y = 52;
  }

  tft.setCursor(8, y);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print(device);

  tft.setCursor(64, y);
  tft.setTextColor(color);
  tft.print(status);

  line++;
}

void drawMicLevel(int level) {
  // Bar level di bawah (y=136..148), lebar 100px
  const int barMaxW = 100;
  const int barX = 14;
  const int barY = 136;
  const int barH = 12;

  int barW = map(level, 0, 1000, 0, barMaxW);
  barW = constrain(barW, 0, barMaxW);

  // Clear area bar
  tft.fillRect(barX, barY, barMaxW, barH, ST77XX_BLACK);

  // Warna berdasarkan level
  if (barW > 0) {
    uint16_t barColor = ST77XX_GREEN;
    if (level > 500) barColor = ST77XX_YELLOW;
    if (level > 800) barColor = ST77XX_RED;
    tft.fillRect(barX, barY, barW, barH, barColor);
  }

  // Border bar
  tft.drawRect(barX, barY, barMaxW, barH, ST77XX_WHITE);
}

// ========== SPEAKER FUNCTIONS ==========
void initSpeaker() {
  i2s_config_t speaker_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t speaker_pins = {
    .bck_io_num = SPEAKER_BCLK,
    .ws_io_num = SPEAKER_LRC,
    .data_out_num = SPEAKER_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_SPEAKER, &speaker_config, 0, NULL);
  i2s_set_pin(I2S_SPEAKER, &speaker_pins);
  i2s_set_clk(I2S_SPEAKER, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}

void playTone(float freq, int duration_ms) {
  const int16_t AMP = 30000;
  const double TWOPI = 6.283185307179586;
  double dphi = TWOPI * freq / SAMPLE_RATE;

  int total_samples = (SAMPLE_RATE * duration_ms) / 1000;
  int samples_written = 0;

  while (samples_written < total_samples) {
    int samples_this_round = min(256, total_samples - samples_written);

    for (int i = 0; i < samples_this_round; i++) {
      phase += dphi;
      if (phase >= TWOPI) phase -= TWOPI;

      int16_t sample = (int16_t)(AMP * sin(phase));
      speaker_buffer[i * 2] = sample;      // Left
      speaker_buffer[i * 2 + 1] = sample;  // Right
    }

    size_t written;
    i2s_write(I2S_SPEAKER, speaker_buffer, samples_this_round * 4, &written, portMAX_DELAY);
    samples_written += samples_this_round;
  }
}

// ========== MICROPHONE FUNCTIONS ==========
void initMicrophone() {
  i2s_config_t mic_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t mic_pins = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD
  };

  i2s_driver_install(I2S_MIC, &mic_config, 0, NULL);
  i2s_set_pin(I2S_MIC, &mic_pins);
  i2s_zero_dma_buffer(I2S_MIC);

  // Flush initial garbage
  for (int i = 0; i < 10; i++) {
    size_t bytes_read;
    i2s_read(I2S_MIC, mic_buffer, sizeof(mic_buffer), &bytes_read, 100);
  }
}

int readMicLevel() {
  size_t bytes_read;
  i2s_read(I2S_MIC, mic_buffer, sizeof(mic_buffer), &bytes_read, 100);

  if (bytes_read == 0) return 0;

  int samples = bytes_read / sizeof(int32_t);
  int64_t sum = 0;

  for (int i = 0; i < samples; i++) {
    int16_t sample = (int16_t)(mic_buffer[i] >> 14);
    sum += abs(sample);
  }

  return sum / samples;
}

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║   ESP32-S3 Complete Hardware Test         ║");
  Serial.println("╚════════════════════════════════════════════╝\n");

  // Initialize LCD
  Serial.print("Initializing LCD... ");
  initLCD();
  Serial.println("OK!");
  updateLCDStatus("LCD:", "OK", ST77XX_GREEN);
  delay(500);

  // Initialize Speaker
  Serial.print("Initializing Speaker... ");
  initSpeaker();
  Serial.println("OK!");
  updateLCDStatus("Speaker:", "OK", ST77XX_GREEN);
  delay(500);

  // Test speaker with beep
  Serial.println("Testing speaker (beep)...");
  updateLCDStatus("Spk Test:", "Beeping...", ST77XX_YELLOW);
  playTone(440, 300);  // A4 note for 300ms
  delay(200);
  playTone(523, 300);  // C5 note for 300ms
  updateLCDStatus("Spk Test:", "Done!", ST77XX_GREEN);
  delay(500);

  // Initialize Microphone
  Serial.print("Initializing Microphone... ");
  initMicrophone();
  Serial.println("OK!");
  updateLCDStatus("Mic:", "OK", ST77XX_GREEN);
  delay(500);

  // Clear & show monitoring screen (disesuaikan 128x160)
  tft.fillRect(5, 52, 118, 70, ST77XX_BLACK);

  tft.setCursor(18, 60);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.print("MONITOR");

  tft.setCursor(10, 86);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Speak/clap to test mic");

  tft.setCursor(10, 100);
  tft.print("Press BOOT for beep");

  tft.setCursor(10, 122);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Mic Level:");

  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║  ALL SYSTEMS READY!                       ║");
  Serial.println("║  - Speak/clap to test microphone          ║");
  Serial.println("║  - Press BOOT button for beep test        ║");
  Serial.println("╚════════════════════════════════════════════╝\n");

  // Configure BOOT button
  pinMode(0, INPUT_PULLUP);
}

// ========== LOOP ==========
void loop() {
  static uint32_t last_update = 0;
  static uint32_t last_beep = 0;
  static bool boot_pressed = false;

  // Check BOOT button (GPIO 0)
  if (digitalRead(0) == LOW && !boot_pressed) {
    boot_pressed = true;

    if (millis() - last_beep > 1000) {  // Debounce
      Serial.println("🔊 Playing test beep...");

      // Show on LCD (area kecil agar muat)
      tft.fillRect(10, 116, 108, 14, ST77XX_BLACK);
      tft.setCursor(18, 118);
      tft.setTextColor(ST77XX_YELLOW);
      tft.setTextSize(1);
      tft.print("Playing beep...");

      playTone(440, 200);
      delay(100);
      playTone(523, 200);

      // Clear message
      tft.fillRect(10, 116, 108, 14, ST77XX_BLACK);

      last_beep = millis();
    }
  } else if (digitalRead(0) == HIGH) {
    boot_pressed = false;
  }

  // Update mic level every 100ms
  if (millis() - last_update > 100) {
    int mic_level = readMicLevel();

    // Update LCD bar
    drawMicLevel(mic_level);

    // Print to serial
    int bar_length = map(mic_level, 0, 1000, 0, 40);
    bar_length = constrain(bar_length, 0, 40);

    Serial.print("[");
    for (int i = 0; i < 40; i++) {
      Serial.print(i < bar_length ? "█" : " ");
    }
    Serial.print("] ");
    Serial.printf("Level: %4d ", mic_level);

    if (mic_level < 100) {
      Serial.println("🔇 Silent");
    } else if (mic_level < 300) {
      Serial.println("🔉 Quiet");
    } else if (mic_level < 600) {
      Serial.println("🔊 Normal");
    } else {
      Serial.println("📢 LOUD!");
    }

    last_update = millis();
  }

  delay(10);
}
