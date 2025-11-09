#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

// Pin ke MAX98357A
#define I2S_DOUT  7   // DIN
#define I2S_BCLK 15   // BCLK
#define I2S_LRC  16   // LRC (WS)

// ===== Konfigurasi I2S =====
static const int SAMPLE_RATE = 44100;

static const i2s_config_t i2s_cfg = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate = SAMPLE_RATE,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
  .communication_format = I2S_COMM_FORMAT_STAND_I2S,
  .intr_alloc_flags = 0,
  .dma_buf_count = 8,
  .dma_buf_len = 256,
  .use_apll = false,
  .tx_desc_auto_clear = true,
  .fixed_mclk = 0
};

static const i2s_pin_config_t pin_cfg = {
  .bck_io_num = I2S_BCLK,
  .ws_io_num = I2S_LRC,
  .data_out_num = I2S_DOUT,
  .data_in_num = I2S_PIN_NO_CHANGE
};

// ===== Tone generator =====
float freq = 440.0f;  // A4

// AMPLITUDE SETTINGS - Pilih salah satu:
// const int16_t AMP = 3000;   // ~9%  - SANGAT PELAN (original)
// const int16_t AMP = 8000;   // ~24% - Pelan
// const int16_t AMP = 16000;  // ~49% - Normal/Medium ✅ RECOMMENDED
const int16_t AMP = 24000;  // ~73% - Keras
// const int16_t AMP = 30000;     // ~92% - SANGAT KERAS 🔊

const size_t N_SAMPLES = 512;
int16_t frame[2 * N_SAMPLES]; // L+R interleaved

double phase = 0.0;
const double TWOPI = 6.283185307179586;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║   ESP32-S3 Speaker Volume Test    ║");
  Serial.println("╚════════════════════════════════════╝");
  Serial.printf("Amplitude: %d / 32767 (%.1f%%)\n", AMP, (AMP / 32767.0) * 100);
  Serial.println();
  
  // Init I2S
  Serial.print("Initializing I2S... ");
  i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_cfg);
  i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
  Serial.println("OK!");
  
  Serial.println("\n🔊 Playing tone...");
  Serial.println("Frequency will alternate every 2 seconds:");
  Serial.println("  440 Hz (A4) <-> 523 Hz (C5)\n");
}

void loop() {
  // Generate sine wave
  double dphi = TWOPI * freq / SAMPLE_RATE;
  
  for (size_t i = 0; i < N_SAMPLES; i++) {
    phase += dphi;
    if (phase >= TWOPI) phase -= TWOPI;
    
    int16_t s = (int16_t)(AMP * sin(phase));
    frame[2*i]   = s; // Left
    frame[2*i+1] = s; // Right
  }

  size_t written = 0;
  i2s_write(I2S_NUM_0, (const char*)frame, sizeof(frame), &written, portMAX_DELAY);

  // Alternate frequency every 2 seconds
  static uint32_t t0 = millis();
  if (millis() - t0 > 2000) {
    t0 = millis();
    freq = (freq == 440.0f) ? 523.25f : 440.0f; // A4 <-> C5
    Serial.printf("🎵 Frequency: %.2f Hz\n", freq);
  }
}