/*
 * ESP32-S3 Complete Hardware Test - NeoPixel Version
 * Board: ESP32-S3 N16R8 with ST7735 1.8" LCD
 * Tests: LCD + Speaker + Microphone + WS2812B (using NeoPixel lib)
 * 
 * Modified for ST7735 1.8" 128x160 SPI LCD
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_NeoPixel.h>
#include <driver/i2s.h>
#include <math.h>

// ========== LCD PINS (ST7735 1.8" 128x160) ==========
#define PIN_TFT_SCLK  21
#define PIN_TFT_MOSI  47
#define PIN_TFT_MISO  -1
#define PIN_TFT_CS    41
#define PIN_TFT_DC    40
#define PIN_TFT_RST   45
#define PIN_TFT_BL    42
#define TFT_WIDTH     128
#define TFT_HEIGHT    160

// ========== SPEAKER PINS ==========
#define SPEAKER_DOUT  7
#define SPEAKER_BCLK  15
#define SPEAKER_LRC   16

// ========== MICROPHONE PINS ==========
#define MIC_WS        4
#define MIC_SCK       5
#define MIC_SD        6

// ========== WS2812B LED (NeoPixel) ==========
#define LED_PIN       48
#define NUM_LEDS      1

// ========== I2S CONFIGURATION ==========
#define SAMPLE_RATE   16000
#define I2S_SPEAKER   I2S_NUM_0
#define I2S_MIC       I2S_NUM_1

// ========== OBJECTS ==========
SPIClass spi(FSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ========== VARIABLES ==========
int32_t mic_buffer[512];
int16_t speaker_buffer[512];
bool speaker_playing = false;
float tone_freq = 440.0f;
double phase = 0.0;
int led_mode = 3;  // 0=off, 1=rainbow, 2=breathing, 3=mic reactive

// ========== LCD FUNCTIONS ==========
void initLCD() {
  spi.begin(PIN_TFT_SCLK, PIN_TFT_MISO, PIN_TFT_MOSI);
  pinMode(PIN_TFT_BL, OUTPUT);
  digitalWrite(PIN_TFT_BL, HIGH);
  
  // Initialize ST7735 1.8" 128x160
  tft.initR(INITR_BLACKTAB);  // Use BLACKTAB for ST7735 1.8"
  tft.setRotation(0);  // Portrait mode: 128x160
  tft.fillScreen(ST77XX_BLACK);
  
  // Draw border
  tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ST77XX_BLUE);
  tft.setTextWrap(false);
  
  // Title
  tft.setCursor(15, 8);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.print("ESP32-S3");
  
  // Subtitle
  tft.setCursor(8, 28);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(1);
  tft.print("HW TEST v2.1");
  
  // Divider line
  tft.drawLine(5, 42, 123, 42, ST77XX_WHITE);
}

void updateLCDStatus(const char* device, const char* status, uint16_t color) {
  static int line = 0;
  int y = 48 + (line * 12);
  
  if (line >= 6) {
    tft.fillRect(5, 48, 118, 72, ST77XX_BLACK);
    line = 0;
    y = 48;
  }
  
  tft.setCursor(8, y);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print(device);
  
  tft.setCursor(65, y);
  tft.setTextColor(color);
  tft.print(status);
  
  line++;
}

void drawMicLevel(int level) {
  int barWidth = map(level, 0, 1000, 0, 108);
  barWidth = constrain(barWidth, 0, 108);
  
  // Clear bar area
  tft.fillRect(10, 142, 108, 10, ST77XX_BLACK);
  
  if (barWidth > 0) {
    uint16_t barColor = ST77XX_GREEN;
    if (level > 500) barColor = ST77XX_YELLOW;
    if (level > 800) barColor = ST77XX_RED;
    
    tft.fillRect(10, 142, barWidth, 10, barColor);
  }
  
  // Draw bar outline
  tft.drawRect(10, 142, 108, 10, ST77XX_WHITE);
}

void updateLEDModeDisplay() {
  tft.fillRect(10, 105, 108, 12, ST77XX_BLACK);
  tft.setCursor(10, 107);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.print("LED: ");
  
  switch(led_mode) {
    case 0: tft.print("OFF"); break;
    case 1: tft.print("Rainbow"); break;
    case 2: tft.print("Breath"); break;
    case 3: tft.print("MicReact"); break;
  }
}

// ========== LED FUNCTIONS (NeoPixel) ==========
void initLED() {
  Serial.print("Initializing WS2812B (NeoPixel) on GPIO 48... ");
  
  strip.begin();
  strip.setBrightness(80);  // 0-255
  strip.clear();
  strip.show();
  
  delay(200);
  
  Serial.println("OK!");
  Serial.println("Testing LED: R-G-B-W...");
  
  // Red
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.show();
  delay(300);
  
  // Green
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.show();
  delay(300);
  
  // Blue
  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.show();
  delay(300);
  
  // White
  strip.setPixelColor(0, strip.Color(255, 255, 255));
  strip.show();
  delay(300);
  
  // Off
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  
  Serial.println("LED test complete!");
}

// Helper function: Convert HSV to RGB
void setPixelColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
  uint32_t color = strip.gamma32(strip.ColorHSV(hue * 65536 / 360, sat, val));
  strip.setPixelColor(0, color);
}

void updateLED(int mic_level) {
  static uint16_t hue = 0;
  static uint8_t brightness = 0;
  static bool breathing_up = true;
  
  switch(led_mode) {
    case 0:  // OFF
      strip.setPixelColor(0, strip.Color(0, 0, 0));
      break;
      
    case 1:  // Rainbow cycle
      hue += 2;
      if (hue >= 360) hue = 0;
      setPixelColorHSV(hue, 255, 255);
      break;
      
    case 2:  // Breathing effect (Blue)
      if (breathing_up) {
        brightness += 5;
        if (brightness >= 250) breathing_up = false;
      } else {
        brightness -= 5;
        if (brightness <= 5) breathing_up = true;
      }
      strip.setPixelColor(0, strip.Color(0, 0, brightness));
      break;
      
    case 3:  // Mic reactive
      {
        uint8_t led_brightness = map(mic_level, 0, 1000, 0, 255);
        led_brightness = constrain(led_brightness, 0, 255);
        
        // Color transition: Green (low) -> Yellow (med) -> Red (high)
        uint8_t red, green, blue;
        
        if (mic_level < 500) {
          // Green to Yellow
          red = map(mic_level, 0, 500, 0, 255);
          green = 255;
          blue = 0;
        } else {
          // Yellow to Red
          red = 255;
          green = map(mic_level, 500, 1000, 255, 0);
          blue = 0;
        }
        
        // Apply brightness
        red = (red * led_brightness) / 255;
        green = (green * led_brightness) / 255;
        blue = (blue * led_brightness) / 255;
        
        strip.setPixelColor(0, strip.Color(red, green, blue));
      }
      break;
  }
  
  strip.show();
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
  speaker_playing = true;
  const int16_t AMP = 27000;
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
      speaker_buffer[i * 2] = sample;
      speaker_buffer[i * 2 + 1] = sample;
    }
    
    size_t written;
    i2s_write(I2S_SPEAKER, speaker_buffer, samples_this_round * 4, &written, portMAX_DELAY);
    samples_written += samples_this_round;
  }
  
  speaker_playing = false;
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
  Serial.println("║   ESP32-S3 Complete Hardware Test v2.1    ║");
  Serial.println("║   ST7735 1.8\" 128x160 LCD Version        ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  // Initialize LCD
  Serial.print("Initializing ST7735 LCD... ");
  initLCD();
  Serial.println("OK!");
  updateLCDStatus("LCD:", "OK", ST77XX_GREEN);
  delay(400);
  
  // Initialize Speaker
  Serial.print("Initializing Speaker... ");
  initSpeaker();
  Serial.println("OK!");
  updateLCDStatus("Speaker:", "OK", ST77XX_GREEN);
  delay(400);
  
  // Initialize Microphone
  Serial.print("Initializing Microphone... ");
  initMicrophone();
  Serial.println("OK!");
  updateLCDStatus("Mic:", "OK", ST77XX_GREEN);
  delay(400);
  
  // Initialize LED (NeoPixel)
  initLED();
  updateLCDStatus("RGB LED:", "OK", ST77XX_GREEN);
  delay(400);
  
  // Test speaker with beep + LED flash
  Serial.println("Testing speaker (beep)...");
  updateLCDStatus("Test:", "Beeping..", ST77XX_YELLOW);
  
  strip.setPixelColor(0, strip.Color(255, 255, 0));  // Yellow
  strip.show();
  playTone(440, 300);
  
  strip.setPixelColor(0, strip.Color(0, 255, 255));  // Cyan
  strip.show();
  delay(200);
  playTone(523, 300);
  
  strip.setPixelColor(0, strip.Color(0, 0, 0));  // Off
  strip.show();
  
  updateLCDStatus("Test:", "Done!", ST77XX_GREEN);
  delay(400);
  
  // Show monitoring screen
  tft.fillRect(5, 48, 118, 72, ST77XX_BLACK);
  
  tft.setCursor(12, 52);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.print("MONITOR");
  
  tft.setCursor(8, 72);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Speak/clap");
  
  tft.setCursor(8, 83);
  tft.print("to test mic");
  
  tft.setCursor(8, 95);
  tft.print("BOOT = Beep");
  
  updateLEDModeDisplay();
  
  tft.setCursor(10, 125);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(1);
  tft.print("Mic Level:");
  
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║  ALL SYSTEMS READY!                       ║");
  Serial.println("║  - Speak/clap to test microphone          ║");
  Serial.println("║  - Press BOOT (short) for beep test       ║");
  Serial.println("║  - Hold BOOT (1s) to change LED mode      ║");
  Serial.println("║                                            ║");
  Serial.println("║  LED Modes:                                ║");
  Serial.println("║    0 = OFF                                 ║");
  Serial.println("║    1 = Rainbow                             ║");
  Serial.println("║    2 = Breathing                           ║");
  Serial.println("║    3 = Mic Reactive (VU Meter) ⭐         ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  pinMode(0, INPUT_PULLUP);
}

// ========== LOOP ==========
void loop() {
  static uint32_t last_update = 0;
  static uint32_t last_beep = 0;
  static uint32_t boot_press_time = 0;
  static bool boot_pressed = false;
  static bool long_press_handled = false;
  
  // Check BOOT button
  if (digitalRead(0) == LOW) {
    if (!boot_pressed) {
      boot_pressed = true;
      boot_press_time = millis();
      long_press_handled = false;
    }
    
    // Long press (>1 second) = change LED mode
    if (!long_press_handled && (millis() - boot_press_time > 1000)) {
      led_mode = (led_mode + 1) % 4;
      
      Serial.printf("\n💡 LED Mode changed to: %d\n", led_mode);
      updateLEDModeDisplay();
      
      // Visual feedback
      strip.setPixelColor(0, strip.Color(255, 255, 255));
      strip.show();
      delay(100);
      strip.setPixelColor(0, strip.Color(0, 0, 0));
      strip.show();
      
      long_press_handled = true;
    }
    
  } else if (boot_pressed) {
    uint32_t press_duration = millis() - boot_press_time;
    
    // Short press (<1 second) = beep test
    if (press_duration < 1000 && !long_press_handled) {
      if (millis() - last_beep > 500) {
        Serial.println("\n🔊 Playing test beep...");
        
        tft.fillRect(10, 105, 108, 12, ST77XX_BLACK);
        tft.setCursor(20, 107);
        tft.setTextColor(ST77XX_YELLOW);
        tft.setTextSize(1);
        tft.print("Beeping...");
        
        strip.setPixelColor(0, strip.Color(255, 0, 255));  // Magenta
        strip.show();
        playTone(440, 200);
        
        delay(100);
        
        strip.setPixelColor(0, strip.Color(0, 255, 255));  // Cyan
        strip.show();
        playTone(523, 200);
        
        strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.show();
        
        updateLEDModeDisplay();
        last_beep = millis();
      }
    }
    
    boot_pressed = false;
  }
  
  // Update mic level and LED every 50ms
  if (millis() - last_update > 50) {
    int mic_level = readMicLevel();
    
    // Update LCD bar
    drawMicLevel(mic_level);
    
    // Update LED based on mode
    updateLED(mic_level);
    
    // Print to serial every 200ms
    static uint32_t last_serial = 0;
    if (millis() - last_serial > 200) {
      int bar_length = map(mic_level, 0, 1000, 0, 40);
      bar_length = constrain(bar_length, 0, 40);
      
      Serial.print("[");
      for (int i = 0; i < 40; i++) {
        Serial.print(i < bar_length ? "█" : " ");
      }
      Serial.print("] ");
      Serial.printf("Lvl:%4d ", mic_level);
      
      if (mic_level < 100) {
        Serial.print("🔇");
      } else if (mic_level < 300) {
        Serial.print("🔉");
      } else if (mic_level < 600) {
        Serial.print("🔊");
      } else {
        Serial.print("📢");
      }
      
      Serial.printf(" | LED:%d", led_mode);
      Serial.println();
      
      last_serial = millis();
    }
    
    last_update = millis();
  }
  
  delay(10);
}
