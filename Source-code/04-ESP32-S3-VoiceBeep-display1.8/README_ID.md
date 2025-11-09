# ESP32-S3 VoiceBeep Display 1.8"

Proyek Arduino untuk menguji hardware ESP32-S3 dengan LCD 1.8", speaker MAX98357A, dan mikrofon INMP441. Program ini menampilkan status hardware pada LCD dan memungkinkan pengujian audio input/output.

## 📋 Gambaran Umum

Proyek ini merupakan **Complete Hardware Test** untuk board ESP32-S3 yang dilengkapi dengan:
- **LCD 1.8"** (ST7735 128x160) untuk menampilkan status dan monitoring
- **Speaker MAX98357A** untuk output audio (beep test)
- **Mikrofon INMP441** untuk input audio dengan visualisasi level
- **Tombol BOOT** untuk trigger test beep

## 🛠️ Spesifikasi Hardware

- Board Goouuu ESP32-S3

- LCD Display  ST7735 1.8" Resolusi: 128x160 pixel

- Gouu Shield ( MAX98357A I2S Amplifier dan  MIC INMP441 I2S Integrated)



## 🔌 Pin Configuration

### LCD Pins (ST7735)
| Pin | Fungsi | GPIO ESP32-S3 |
|-----|--------|---------------|
| SCLK | Serial Clock | 21 |
| MOSI | Master Out Slave In | 47 |
| CS   | Chip Select | 41 |
| DC   | Data/Command | 40 |
| RST  | Reset | 45 |
| BL   | Backlight | 42 |

### Speaker Pins (MAX98357A)
| Pin | Fungsi | GPIO ESP32-S3 |
|-----|--------|---------------|
| DIN  | Data In | 7 |
| BCLK | Bit Clock | 15 |
| LRC  | Left/Right Clock | 16 |

### Microphone Pins (INMP441)
| Pin | Fungsi | GPIO ESP32-S3 |
|-----|--------|---------------|
| WS   | Word Select | 4 |
| SCK  | Serial Clock | 5 |
| SD   | Serial Data | 6 |

### Tombol
| Tombol | Fungsi | GPIO ESP32-S3 |
|--------|--------|---------------|
| BOOT  | Trigger Beep Test | 0 |

## 📦 Instalasi dan Setup

### Prerequisites
1. **Arduino IDE** versi 2.0 atau lebih baru
2. **ESP32 Board Package** terinstal
3. **Library Arduino** yang diperlukan

### Library yang Diperlukan

Install library berikut melalui Arduino IDE Library Manager:

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <driver/i2s.h>
#include <math.h>
```

### Langkah Instalasi

1. **Install ESP32 Board Package**:
   - Buka Arduino IDE → File → Preferences
   - Tambahkan URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Board Manager → Cari "ESP32" → Install

2. **Install Library**:
   - Sketch → Include Library → Manage Libraries
   - Cari dan install:
     - "Adafruit GFX Library"
     - "Adafruit ST7735 and ST7789 Library"

3. **Konfigurasi Board**:
   - Tools → Board → ESP32 Arduino → "ESP32S3 Dev Module"
   - Flash Mode: QIO
   - Flash Size: 16MB
   - Partition Scheme: Default 16MB Flash/8MB PSRAM
   - USB CDC On Boot: Enabled
   - CPU Frequency: 240MHz

4. **Upload Kode**:
   - Buka file `04-ESP32-S3-VoiceBeep-display1.8.ino`
   - Pilih port COM yang sesuai
   - Upload kode ke board

## 🎮 Cara Penggunaan

### Startup Sequence
Setelah diupload, program akan melakukan inisialisasi hardware secara berurutan:

1. LCD Initialization - Menampilkan header "ESP32-S3 HW TEST"
2. Speaker Test - Memainkan dua nada beep (A4 dan C5)
3. Microphone Initialization - Siap menerima input audio
4. Monitoring Mode - Menampilkan level mikrofon secara real-time

### Fungsi Tombol
- **Tombol BOOT (GPIO 0)**: Tekan untuk memainkan test beep
  - Akan memainkan nada A4 (440Hz) dan C5 (523Hz)
  - Debounce time: 1 detik

### Monitoring Display
LCD akan menampilkan:
- Header: "ESP32-S3" dan "HW TEST"
- Status Area: Status setiap komponen hardware
- Mic Level Bar: Visualisasi level input mikrofon
  - Hijau: Level rendah
  - Kuning: Level sedang  
  - Merah: Level tinggi/keras

### Serial Monitor
Buka Serial Monitor (115200 baud) untuk melihat:
- Status inisialisasi hardware
- Real-time mic level dengan visualisasi bar
- Kategori level audio (Silent, Quiet, Normal, LOUD!)

## 🔧 Troubleshooting

### LCD Tidak Menyala
- Periksa koneksi pin backlight (GPIO 42)
- Pastikan library ST7735 terinstal dengan benar
- Cek konfigurasi SPI pins

### Speaker Tidak Bersuara
- Verifikasi koneksi I2S speaker pins
- Pastikan MAX98357A mendapat power supply yang cukup
- Cek konfigurasi I2S channel 0

### Mikrofon Tidak Bekerja
- Periksa koneksi INMP441
- Pastikan I2S channel 1 terkonfigurasi dengan benar
- Cek sample rate dan format (32-bit mono)

### Tombol BOOT Tidak Responsif
- Pastikan menggunakan internal pull-up resistor
- Periksa koneksi GPIO 0
- Cek debounce timing

## 📊 Fitur Teknis

### Audio Processing
- **Sample Rate**: 16kHz
- **Speaker**: 16-bit stereo
- **Microphone**: 32-bit mono
- **Buffer Size**: 512 samples

### Display Features
- **Rotation**: 2 (sesuai orientasi ST7789)
- **SPI Speed**: 20MHz
- **Color Scheme**: Optimized untuk ST7735

### Performance
- **Update Rate**: 10Hz untuk mic level
- **Debounce Time**: 1 detik untuk tombol
- **Memory Usage**: Optimized untuk ESP32-S3 dengan PSRAM

## 🚀 Pengembangan Lebih Lanjut

Proyek ini dapat dikembangkan untuk:
- Voice recognition dengan AI
- Audio visualizer yang lebih kompleks
- IoT applications dengan audio streaming
- Music player dengan display kontrol

## 📄 Lisensi

Proyek ini open source dan dapat digunakan untuk keperluan edukasi dan pengembangan.

## 🤝 Kontribusi

Silakan fork dan submit pull request untuk perbaikan atau fitur tambahan.

---

**Dibuat dengan ❤️ untuk komunitas ESP32 Indonesia**