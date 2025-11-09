/*
 * ESP32-S3 Voice Recorder - 5 Second Record & Play
 * WITH WS2812B LED INDICATOR
 * Modified for ST7735 1.8" 128x160 LCD
 * 
 * FEATURES:
 * - 500ms delay setelah button press (skip button click sound)
 * - Display no flicker (selective update)
 * - 5 second recording time
 * - WS2812B LED: RED (recording), GREEN (playing), OFF (idle)
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_NeoPixel.h>
#include <driver/i2s.h>

// ========== LCD PINS (ST7735 1.8" 128x160) ==========
#define PIN_TFT_SCLK  21
#define PIN_TFT_MOSI  47
#define PIN_TFT_CS    41
#define PIN_TFT_DC    40
#define PIN_TFT_RST   45
#define PIN_TFT_BL    42
#define TFT_WIDTH     128
#define TFT_HEIGHT    160

// ========== AUDIO PINS ==========
#define SPEAKER_DOUT  7
#define SPEAKER_BCLK  15
#define SPEAKER_LRC   16

#define MIC_WS        4
#define MIC_SCK       5
#define MIC_SD        6

// ========== WS2812B LED ==========
#define LED_PIN       48
#define LED_COUNT     1

// ========== AUDIO SETTINGS ==========
#define SAMPLE_RATE   16000
#define RECORD_TIME   5         // 5 seconds 
#define BUFFER_SIZE   512
#define BUTTON_DELAY  500       // 500ms delay to skip button click sound

// Calculate total samples for 5 seconds
#define TOTAL_SAMPLES (SAMPLE_RATE * RECORD_TIME)

// ========== I2S PORTS ==========
#define I2S_SPEAKER   I2S_NUM_0
#define I2S_MIC       I2S_NUM_1

// ========== OBJECTS ==========
SPIClass spi(FSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ========== BUFFERS ==========
int32_t mic_buffer[BUFFER_SIZE];
int16_t speaker_buffer[BUFFER_SIZE * 2]; // stereo

// Recording buffer - 5 seconds at 16kHz = 80000 samples
int16_t* recording_buffer = NULL;
int recorded_samples = 0;

// ========== STATE ==========
enum State {
  IDLE,
  WAITING,    // NEW: Wait 500ms after button press
  RECORDING,
  PLAYING
};

State current_state = IDLE;
State last_state = IDLE;
bool boot_pressed = false;
uint32_t state_start_time = 0;
int playback_position = 0;

// Display update tracking
float last_countdown = -1;
int last_progress = -1;

// ========== LED FUNCTIONS ==========
void setLED(uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
}

void ledOff() {
  setLED(0, 0, 0);
}

void ledRed() {
  setLED(255, 0, 0);
}

void ledGreen() {
  setLED(0, 255, 0);
}

void ledYellow() {
  setLED(255, 150, 0);
}

// ========== LCD FUNCTIONS ==========
void initLCD() {
  spi.begin(PIN_TFT_SCLK, -1, PIN_TFT_MOSI);
  pinMode(PIN_TFT_BL, OUTPUT);
  digitalWrite(PIN_TFT_BL, HIGH);
  
  // Initialize ST7735 1.8" 128x160
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);  // Portrait: 128x160
  tft.fillScreen(ST77XX_BLACK);
  
  // Title - DRAW ONCE, NEVER CLEAR
  tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ST77XX_BLUE);
  tft.setCursor(8, 8);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.print("VOICE RECORDER");
  
  tft.drawLine(5, 22, 123, 22, ST77XX_WHITE);
}

void drawIdleScreen() {
  // Only draw once when entering IDLE state
  tft.fillRect(5, 28, 118, 127, ST77XX_BLACK);
  
  tft.setTextSize(2);
  tft.setCursor(30, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("READY");
  
  tft.setTextSize(1);
  tft.setCursor(8, 60);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Press BOOT to");
  tft.setCursor(25, 72);
  tft.print("record 5 sec");
  
  tft.setCursor(8, 92);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Record time:");
  tft.setCursor(15, 103);
  tft.print("5 seconds");
  
  if (recorded_samples > 0) {
    tft.setCursor(8, 120);
    tft.setTextColor(ST77XX_GREEN);
    tft.print("Last: ");
    tft.printf("%.1fs", recorded_samples / (float)SAMPLE_RATE);
  }
  
  tft.setCursor(10, 143);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("BOOT = Record");
}

void drawWaitingScreen() {
  tft.fillRect(5, 28, 118, 127, ST77XX_BLACK);
  
  tft.setTextSize(2);
  tft.setCursor(15, 40);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("GETREADY");
  
  tft.setTextSize(1);
  tft.setCursor(10, 70);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Recording in..");
  
  // Countdown will be updated separately
}

void updateWaitingCountdown() {
  uint32_t elapsed = millis() - state_start_time;
  float remaining = (BUTTON_DELAY - elapsed) / 1000.0;
  
  if (remaining < 0) remaining = 0;
  
  // Only update if changed
  if (abs(remaining - last_countdown) > 0.05) {
    // Clear only countdown area
    tft.fillRect(40, 90, 50, 30, ST77XX_BLACK);
    
    tft.setTextSize(3);
    tft.setCursor(48, 95);
    tft.setTextColor(ST77XX_YELLOW);
    tft.printf("%.1f", remaining);
    
    last_countdown = remaining;
  }
}

void drawRecordingScreen() {
  tft.fillRect(5, 28, 118, 127, ST77XX_BLACK);
  
  tft.setTextSize(2);
  tft.setCursor(8, 35);
  tft.setTextColor(ST77XX_RED);
  tft.print("RECORDING");
  
  // Draw static progress bar frame
  tft.drawRect(10, 135, 108, 14, ST77XX_WHITE);
  
  last_countdown = -1;  // Reset for fresh update
  last_progress = -1;
}

void updateRecordingProgress() {
  uint32_t elapsed = millis() - state_start_time;
  float remaining = RECORD_TIME - (elapsed / 1000.0);
  if (remaining < 0) remaining = 0;
  
  // Update countdown only if changed
  if (abs(remaining - last_countdown) > 0.05) {
    tft.fillRect(35, 70, 60, 35, ST77XX_BLACK);
    tft.setTextSize(3);
    tft.setCursor(48, 75);
    tft.setTextColor(ST77XX_RED);
    tft.printf("%.1f", remaining);
    last_countdown = remaining;
  }
  
  // Update progress bar
  int progress = map(elapsed, 0, RECORD_TIME * 1000, 0, 106);
  progress = constrain(progress, 0, 106);
  
  if (progress != last_progress) {
    tft.fillRect(11, 136, 106, 12, ST77XX_BLACK);
    tft.fillRect(11, 136, progress, 12, ST77XX_RED);
    last_progress = progress;
  }
  
  // Blinking REC indicator
  static uint32_t last_blink = 0;
  if (millis() - last_blink > 500) {
    static bool blink_state = false;
    tft.fillCircle(110, 35, 6, blink_state ? ST77XX_RED : ST77XX_BLACK);
    tft.drawCircle(110, 35, 6, ST77XX_WHITE);
    blink_state = !blink_state;
    last_blink = millis();
  }
}

void drawPlayingScreen() {
  tft.fillRect(5, 28, 118, 127, ST77XX_BLACK);
  
  tft.setTextSize(2);
  tft.setCursor(25, 35);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("PLAYING");
  
  // Draw static progress bar frame
  tft.drawRect(10, 135, 108, 14, ST77XX_WHITE);
  
  last_progress = -1;  // Reset
}

void updatePlayingProgress() {
  float progress_pct = (playback_position * 100.0) / recorded_samples;
  
  // Update percentage
  tft.fillRect(35, 70, 60, 35, ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, 80);
  tft.setTextColor(ST77XX_GREEN);
  tft.printf("%.0f%%", progress_pct);
  
  // Update progress bar
  int progress = map(playback_position, 0, recorded_samples, 0, 106);
  progress = constrain(progress, 0, 106);
  
  if (progress != last_progress) {
    tft.fillRect(11, 136, 106, 12, ST77XX_BLACK);
    tft.fillRect(11, 136, progress, 12, ST77XX_GREEN);
    last_progress = progress;
  }
}

// ========== AUDIO FUNCTIONS ==========
void initSpeaker() {
  i2s_config_t speaker_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
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
}

void initMicrophone() {
  i2s_config_t mic_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
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
  
  // Flush garbage
  for (int i = 0; i < 10; i++) {
    size_t bytes_read;
    i2s_read(I2S_MIC, mic_buffer, sizeof(mic_buffer), &bytes_read, 100);
  }
}

// ========== RECORDING FUNCTIONS ==========
void startWaiting() {
  Serial.println("\n⏳ WAITING (skip button click)...");
  current_state = WAITING;
  state_start_time = millis();
  drawWaitingScreen();
  ledYellow();  // Yellow LED saat waiting
}

void startRecording() {
  Serial.println("\n🔴 RECORDING START");
  
  current_state = RECORDING;
  recorded_samples = 0;
  state_start_time = millis();
  
  // Allocate buffer if not already allocated
  if (recording_buffer == NULL) {
    recording_buffer = (int16_t*)malloc(TOTAL_SAMPLES * sizeof(int16_t));
    if (recording_buffer == NULL) {
      Serial.println("❌ Failed to allocate recording buffer!");
      current_state = IDLE;
      drawIdleScreen();
      ledOff();
      return;
    }
  }
  
  // Clear buffer
  memset(recording_buffer, 0, TOTAL_SAMPLES * sizeof(int16_t));
  
  // Flush mic buffer to remove any residual button click
  for (int i = 0; i < 5; i++) {
    size_t bytes_read;
    i2s_read(I2S_MIC, mic_buffer, sizeof(mic_buffer), &bytes_read, 100);
  }
  
  Serial.printf("Recording %d samples (5 seconds)...\n", TOTAL_SAMPLES);
  drawRecordingScreen();
  ledRed();  // RED LED saat recording
}

void recordAudio() {
  size_t bytes_read;
  i2s_read(I2S_MIC, mic_buffer, sizeof(mic_buffer), &bytes_read, portMAX_DELAY);
  
  if (bytes_read == 0) return;
  
  int samples = bytes_read / sizeof(int32_t);
  
  for (int i = 0; i < samples && recorded_samples < TOTAL_SAMPLES; i++) {
    // Convert 32-bit to 16-bit
    int16_t sample = (int16_t)(mic_buffer[i] >> 14);
    
    // Apply gain (2x amplification)
    sample = sample * 2;
    
    // Limiter
    if (sample > 16000) sample = 16000;
    if (sample < -16000) sample = -16000;
    
    recording_buffer[recorded_samples++] = sample;
  }
  
  // Check if recording complete
  if (recorded_samples >= TOTAL_SAMPLES || (millis() - state_start_time) >= (RECORD_TIME * 1000)) {
    Serial.printf("✅ Recording complete! Captured %d samples\n", recorded_samples);
    current_state = PLAYING;
    playback_position = 0;
    drawPlayingScreen();
    ledGreen();  // GREEN LED saat playing
    delay(300); // Short pause before playback
  }
}

void playbackAudio() {
  if (playback_position >= recorded_samples) {
    // Playback complete
    Serial.println("✅ Playback complete!\n");
    current_state = IDLE;
    playback_position = 0;
    drawIdleScreen();
    ledOff();  // LED OFF saat idle
    return;
  }
  
  // Prepare buffer for playback
  int samples_to_play = min(BUFFER_SIZE, recorded_samples - playback_position);
  
  for (int i = 0; i < samples_to_play; i++) {
    int16_t sample = recording_buffer[playback_position + i];
    speaker_buffer[i * 2] = sample;      // Left
    speaker_buffer[i * 2 + 1] = sample;  // Right
  }
  
  size_t bytes_written;
  i2s_write(I2S_SPEAKER, speaker_buffer, samples_to_play * 4, &bytes_written, portMAX_DELAY);
  
  playback_position += samples_to_play;
}

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔═══════════════════════════════════════════╗");
  Serial.println("║   ESP32-S3 Voice Recorder (5 seconds)     ║");
  Serial.println("║   ST7735 1.8\" 128x160 LCD Version        ║");
  Serial.println("╚═══════════════════════════════════════════╝\n");
  
  // Initialize WS2812B LED
  Serial.print("Initializing WS2812B LED... ");
  strip.begin();
  strip.setBrightness(50);  // Set brightness (0-255)
  ledOff();
  Serial.println("OK!");
  
  // Initialize LCD
  Serial.print("Initializing ST7735 LCD... ");
  initLCD();
  Serial.println("OK!");
  
  tft.setCursor(10, 70);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(1);
  tft.print("Initializing");
  tft.setCursor(25, 82);
  tft.print("audio...");
  
  // Initialize audio
  Serial.print("Initializing Speaker... ");
  initSpeaker();
  Serial.println("OK!");
  
  Serial.print("Initializing Microphone... ");
  initMicrophone();
  Serial.println("OK!");
  
  // Setup BOOT button
  pinMode(0, INPUT_PULLUP);
  
  Serial.println("\n╔═══════════════════════════════════════════╗");
  Serial.println("║  READY!                                   ║");
  Serial.println("║  Press BOOT button to record 5 seconds    ║");
  Serial.println("║  LED: RED=Recording, GREEN=Playing        ║");
  Serial.println("╚═══════════════════════════════════════════╝\n");
  
  drawIdleScreen();
  ledOff();  // Start with LED OFF
  delay(500);
}

// ========== LOOP ==========
void loop() {
  static uint32_t last_display_update = 0;
  
  // Check BOOT button (only when idle)
  if (current_state == IDLE) {
    if (digitalRead(0) == LOW && !boot_pressed) {
      boot_pressed = true;
      startWaiting();
    } else if (digitalRead(0) == HIGH) {
      boot_pressed = false;
    }
  }
  
  // State machine
  switch(current_state) {
    case IDLE:
      // Do nothing, wait for button press
      break;
      
    case WAITING:
      // Wait for button delay to expire
      if (millis() - state_start_time >= BUTTON_DELAY) {
        startRecording();
      } else {
        updateWaitingCountdown();
      }
      break;
      
    case RECORDING:
      recordAudio();
      updateRecordingProgress();
      break;
      
    case PLAYING:
      playbackAudio();
      if (millis() - last_display_update > 100) {
        updatePlayingProgress();
        last_display_update = millis();
      }
      break;
  }
  
  // Track state changes
  last_state = current_state;
}
