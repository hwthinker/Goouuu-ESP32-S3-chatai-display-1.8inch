#include <driver/i2s.h>

#define I2S_WS    4
#define I2S_SCK   5
#define I2S_SD    6

#define I2S_PORT      I2S_NUM_0
#define SAMPLE_RATE   16000
#define BUFFER_SIZE   512

int32_t raw_samples[BUFFER_SIZE];

// Baseline untuk deteksi perubahan
int32_t baseline_avg = 0;
int sample_count = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║   INMP441 DEBUG - Detect Sound Changes    ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  // I2S setup
  i2s_config_t i2s_config = {
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
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
  
  delay(500);
  
  // Warmup & flush
  Serial.print("Warming up");
  for(int i = 0; i < 10; i++) {
    size_t bytes_read;
    i2s_read(I2S_PORT, raw_samples, sizeof(raw_samples), &bytes_read, 100);
    Serial.print(".");
  }
  Serial.println(" Done!\n");
  
  // Measure baseline (ambient noise)
  Serial.println("📊 Measuring baseline noise...");
  Serial.println("🤫 STAY QUIET for 5 seconds!\n");
  
  int64_t total_baseline = 0;
  int baseline_samples = 0;
  
  for(int i = 0; i < 50; i++) {
    size_t bytes_read;
    i2s_read(I2S_PORT, raw_samples, sizeof(raw_samples), &bytes_read, portMAX_DELAY);
    
    int samples = bytes_read / sizeof(int32_t);
    for(int j = 0; j < samples; j++) {
      int16_t sample = (int16_t)(raw_samples[j] >> 14);
      total_baseline += abs(sample);
      baseline_samples++;
    }
    
    if(i % 10 == 0) Serial.print(".");
  }
  
  baseline_avg = total_baseline / baseline_samples;
  
  Serial.println(" Done!");
  Serial.printf("\n✓ Baseline noise level: %d\n", baseline_avg);
  Serial.printf("✓ Detection threshold: %d (baseline + 100)\n\n", baseline_avg + 100);
  
  if(baseline_avg < 50) {
    Serial.println("⚠️  WARNING: Baseline is TOO LOW!");
    Serial.println("   This usually means:");
    Serial.println("   - L/R pin is NOT grounded properly");
    Serial.println("   - Mic is not receiving power");
    Serial.println("   - Mic is faulty\n");
  } else if(baseline_avg > 800) {
    Serial.println("⚠️  WARNING: Baseline is TOO HIGH!");
    Serial.println("   This usually means:");
    Serial.println("   - Too much electrical noise");
    Serial.println("   - L/R pin is floating (not grounded)");
    Serial.println("   - Board has interference\n");
  } else {
    Serial.println("✓ Baseline looks reasonable!\n");
  }
  
  Serial.println("╔════════════════════════════════════════════╗");
  Serial.println("║  NOW MAKE SOUNDS! (clap, speak, knock)    ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  delay(1000);
}

void loop() {
  size_t bytes_read = 0;
  i2s_read(I2S_PORT, raw_samples, sizeof(raw_samples), &bytes_read, portMAX_DELAY);
  
  int samples = bytes_read / sizeof(int32_t);
  
  // Calculate current level
  int64_t sum = 0;
  int16_t max_val = -32768;
  int16_t min_val = 32767;
  
  for (int i = 0; i < samples; i++) {
    int16_t sample = (int16_t)(raw_samples[i] >> 14);
    sum += abs(sample);
    if (sample > max_val) max_val = sample;
    if (sample < min_val) min_val = sample;
  }
  
  int32_t current_avg = sum / samples;
  int32_t p2p = max_val - min_val;
  
  // Detect significant change from baseline
  int32_t difference = current_avg - baseline_avg;
  
  // Visual indicator
  int bar = map(current_avg, 0, 1500, 0, 30);
  bar = constrain(bar, 0, 30);
  
  Serial.print("[");
  for(int i = 0; i < 30; i++) {
    Serial.print(i < bar ? "█" : " ");
  }
  Serial.print("] ");
  
  Serial.printf("Lvl:%4d Base:%4d Diff:%+4d P2P:%5d ", 
                current_avg, baseline_avg, difference, p2p);
  
  // Detection logic
  if(difference > 150) {
    Serial.print("🔴 SOUND DETECTED!");
  } else if(difference > 80) {
    Serial.print("🟡 Small change");
  } else if(difference < -80) {
    Serial.print("🔵 Below baseline");
  } else {
    Serial.print("⚪ Ambient");
  }
  
  Serial.println();
  
  delay(50);
}
