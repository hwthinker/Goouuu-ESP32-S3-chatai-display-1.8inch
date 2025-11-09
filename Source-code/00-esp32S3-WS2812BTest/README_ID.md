# 🌈 ESP32-S3 WS2812B LED Test - Panduan Lengkap untuk Pemula

## 📋 Daftar Isi
- [Tentang Program](#-tentang-program)
- [Hardware yang Dibutuhkan](#-hardware-yang-dibutuhkan)
- [Koneksi Hardware](#-koneksi-hardware)
- [Instalasi Software](#-instalasi-software)
- [Cara Upload Program](#-cara-upload-program)
- [Cara Menggunakan](#-cara-menggunakan)
- [Troubleshooting](#-troubleshooting)
- [Modifikasi Program](#-modifikasi-program)

---

## 🎯 Tentang Program

Program ini adalah **tes LED WS2812B** (NeoPixel) untuk ESP32-S3 yang akan:
- ✅ Mengecek apakah LED terpasang dengan benar
- ✅ Menampilkan urutan warna: Merah → Hijau → Biru → Putih → Mati
- ✅ Menjalankan efek rainbow cycling (pelangi berputar)
- ✅ Menampilkan status di Serial Monitor

**Fitur Utama:**
- 🎨 Test 4 warna dasar (RGBW)
- 🌈 Rainbow cycle animation
- 💡 Brightness control (80/255)
- 📊 Serial Monitor feedback
- 🎛️ Mudah dimodifikasi

---

## 🛠️ Hardware yang Dibutuhkan

### 1. **ESP32-S3 Board** dengan internal WS2812BLED
   - ESP32-S3 DevKit atau sejenisnya
   - Pastikan punya port USB untuk upload

### 2. **Kabel USB**
   - Untuk koneksi ESP32-S3 ke komputer

---

## 🔌 Koneksi Hardware

### Diagram Koneksi WS2812B ke ESP32-S3

```
WS2812B Pin    →    ESP32-S3 Pin
─────────────────────────────────
VCC / VDD      →    5V  (atau 3.3V untuk 1 LED)
GND            →    GND
DI / DIN       →    GPIO 48
```

## 💻 Instalasi Software

### Langkah 1: Install Arduino IDE
1. Download **Arduino IDE 2.x** dari: https://www.arduino.cc/en/software
2. Install sesuai sistem operasi (Windows/Mac/Linux)

### Langkah 2: Install Board ESP32-S3
1. Buka Arduino IDE
2. Klik **File** → **Preferences**
3. Di bagian **Additional Boards Manager URLs**, tambahkan:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Klik **OK**
5. Buka **Tools** → **Board** → **Boards Manager**
6. Cari "**esp32**" oleh Espressif Systems
7. Klik **Install** (versi 2.0.0 atau lebih baru)

### Langkah 3: Install Library FastLED
Program ini memerlukan library **FastLED** untuk kontrol LED:

1. Buka **Tools** → **Manage Libraries** (atau Ctrl+Shift+I)
2. Cari "**FastLED**"
3. Pilih **FastLED by Daniel Garcia**
4. Klik **Install**
5. Tunggu hingga selesai

✅ **Library yang dibutuhkan:**
- **FastLED** (versi 3.5.0 atau lebih baru)

---

## ⬆️ Cara Upload Program

### Langkah 1: Buka File Program
1. Buka file **`00-esp32S3-WS2812BTest.ino`** di Arduino IDE
2. Atau copy-paste kode ke sketch baru

### Langkah 2: Pilih Board
1. Klik **Tools** → **Board** → **ESP32 Arduino**
2. Pilih **ESP32S3 Dev Module**

### Langkah 3: Konfigurasi Board
Set konfigurasi seperti ini:
```
Tools →
  ├─ USB CDC On Boot:      "Enabled"
  ├─ CPU Frequency:        "240MHz (WiFi)"
  ├─ Flash Mode:           "QIO 80MHz"
  ├─ Flash Size:           "16MB (128Mb)" (sesuaikan board Anda)
  ├─ Partition Scheme:     "Default 4MB with spiffs"
  ├─ PSRAM:                "Disabled" (atau "OPI PSRAM" jika ada)
  └─ Upload Speed:         "921600"
```

### Langkah 4: Pilih Port
1. Hubungkan ESP32-S3 ke komputer via USB
2. **Tools** → **Port** → Pilih port yang muncul
   - Windows: `COM3`, `COM4`, dll
   - Mac/Linux: `/dev/ttyUSB0`, `/dev/cu.usbserial-xxx`

### Langkah 5: Upload!
1. Klik tombol **Upload** (ikon panah →)
2. Tunggu hingga proses selesai
3. Jika ada error "Failed to connect":
   - Tekan dan tahan tombol **BOOT** di ESP32-S3
   - Klik **Upload** lagi
   - Lepas tombol BOOT setelah "Connecting..."

---

## 🎮 Cara Menggunakan

### Langkah 1: Buka Serial Monitor
1. Setelah upload selesai, klik **Tools** → **Serial Monitor**
2. Pastikan baud rate di kanan bawah adalah **115200**

### Langkah 2: Program Berjalan Otomatis!

#### **Fase 1: Test Sequence (10 detik)**
Program akan menampilkan urutan warna:

```
╔════════════════════════════════════╗
║   WS2812B LED Safe Test           ║
╚════════════════════════════════════╝
Using GPIO 48 for LED data

Testing LED sequence...

🔴 RED      ← LED menyala MERAH (1 detik)
🟢 GREEN    ← LED menyala HIJAU (1 detik)
🔵 BLUE     ← LED menyala BIRU (1 detik)
⚪ WHITE    ← LED menyala PUTIH (1 detik)
⚫ OFF      ← LED mati (0.5 detik)

✅ LED Test Complete!
Starting rainbow cycle...
```

#### **Fase 2: Rainbow Cycle (Loop Forever)**
Setelah test sequence, LED akan menampilkan efek rainbow (pelangi berputar):

```
🌈 Rainbow - Hue: 0
🌈 Rainbow - Hue: 40
🌈 Rainbow - Hue: 80
🌈 Rainbow - Hue: 120
🌈 Rainbow - Hue: 160
...
```

LED akan berubah warna secara halus dari:
- **Merah** → **Oranye** → **Kuning** → **Hijau** → **Cyan** → **Biru** → **Ungu** → **Magenta** → **Merah** (repeat)

---

## 🔧 Troubleshooting

### ❌ Problem: LED tidak menyala sama sekali

**Kemungkinan Penyebab & Solusi:**

1. **Koneksi salah**
   - ✅ Cek koneksi VCC ke 5V (atau 3.3V)
   - ✅ Cek koneksi GND ke GND
   - ✅ Cek koneksi DI/DIN ke GPIO 48
   - ✅ Pastikan tidak ada kabel yang longgar

2. **Power tidak cukup**
   - ✅ Jika >5 LED, gunakan power supply eksternal 5V
   - ✅ Sambungkan GND power supply ke GND ESP32

3. **LED rusak**
   - ✅ Coba LED lain jika ada
   - ✅ Cek dengan multimeter, VCC harus 5V

4. **GPIO salah**
   - ✅ Pastikan menggunakan GPIO 48 (bukan pin lain)
   - ✅ Coba ganti ke GPIO lain (edit kode, line 3)

---

### ❌ Problem: LED hanya menyala 1 warna (terus merah/hijau/biru)

**Solusi:**
1. **Library FastLED tidak terinstall**
   - Install library FastLED (lihat [Instalasi Software](#instalasi-software))

2. **Color order salah**
   - Coba ganti `COLOR_ORDER` di kode:
   ```cpp
   #define COLOR_ORDER   GRB  // Coba: RGB, BRG, BGR
   ```

3. **LED tipe lain**
   - Beberapa LED pakai protokol berbeda
   - Coba ganti `LED_TYPE`:
   ```cpp
   #define LED_TYPE      WS2812B  // Atau: WS2811, SK6812
   ```

---

### ❌ Problem: LED berkedip-kedip tidak stabil

**Kemungkinan Penyebab:**
1. **Kabel jumper terlalu panjang**
   - Gunakan kabel maksimal 15cm
   - Atau solder langsung

2. **Power tidak stabil**
   - Tambahkan capacitor 100-1000µF di VCC-GND
   - Gunakan power supply yang baik

3. **Interference**
   - Tambahkan resistor 330Ω di data line
   - Jauhkan dari motor atau perangkat berisik

---

### ❌ Problem: Upload gagal "Failed to connect"

**Solusi:**
1. Tekan dan tahan tombol **BOOT** di ESP32-S3
2. Klik **Upload** di Arduino IDE
3. Lepas BOOT setelah muncul "Connecting..."
4. Tunggu proses selesai

---

### ❌ Problem: Serial Monitor tidak muncul output

**Solusi:**
1. ✅ Pastikan baud rate = **115200**
2. ✅ Pilih port yang benar di **Tools** → **Port**
3. 🔄 Tekan tombol **RST** di ESP32-S3
4. 🔌 Coba cabut-pasang USB

---

### ❌ Problem: Warna tidak sesuai (merah jadi hijau, dll)

**Solusi:**
Ubah `COLOR_ORDER` di baris 6:
```cpp
#define COLOR_ORDER   GRB  // Default

// Coba salah satu ini:
#define COLOR_ORDER   RGB
#define COLOR_ORDER   BRG
#define COLOR_ORDER   BGR
#define COLOR_ORDER   RBG
#define COLOR_ORDER   GBR
```

Upload ulang setelah ubah.

---

## 🎨 Modifikasi Program

### 1. Ubah GPIO Pin LED

Jika LED terhubung ke GPIO lain (bukan 48):

```cpp
// Baris 3 - Ubah angka sesuai GPIO yang dipakai
#define LED_DATA_PIN  48   // ← Ubah ini (misal: 2, 5, 16, dll)
```

---

### 2. Ubah Jumlah LED

Jika menggunakan LED strip dengan banyak LED:

```cpp
// Baris 4 - Ubah jumlah LED
#define NUM_LEDS      1    // ← Ubah (misal: 8, 16, 30, 60, dll)
```

**Contoh untuk 8 LED strip:**
```cpp
#define NUM_LEDS      8
```

---

### 3. Ubah Brightness (Kecerahan)

```cpp
// Di dalam setup(), baris 22
FastLED.setBrightness(80);  // ← Ubah (0-255)
```

**Rekomendasi brightness:**
- **50** = Redup (hemat power)
- **80** = Sedang (default, bagus untuk 1 LED)
- **128** = Terang
- **255** = Maksimal (konsumsi power tinggi!)

---

### 4. Ubah Kecepatan Rainbow

```cpp
// Di dalam loop(), baris 66
hue += 2;  // ← Ubah angka

// Kecepatan:
hue += 1;  // Lambat
hue += 2;  // Sedang (default)
hue += 5;  // Cepat
hue += 10; // Sangat cepat
```

---

### 5. Tambah Warna Baru di Test Sequence

Tambahkan kode di dalam `setup()`:

```cpp
// Setelah baris 50 (sebelum OFF), tambahkan:

// Test Yellow
Serial.println("🟡 YELLOW");
leds[0] = CRGB::Yellow;
FastLED.show();
delay(1000);

// Test Cyan
Serial.println("🟦 CYAN");
leds[0] = CRGB::Cyan;
FastLED.show();
delay(1000);

// Test Magenta
Serial.println("🟣 MAGENTA");
leds[0] = CRGB::Magenta;
FastLED.show();
delay(1000);
```

**Daftar warna yang tersedia:**
- `CRGB::Red`, `CRGB::Green`, `CRGB::Blue`
- `CRGB::Yellow`, `CRGB::Cyan`, `CRGB::Magenta`
- `CRGB::White`, `CRGB::Black` (mati)
- `CRGB::Orange`, `CRGB::Purple`, `CRGB::Pink`

---

### 6. Ubah Efek Loop (Bukan Rainbow)

#### **Efek Breathing (Berkedip pelan):**
```cpp
void loop() {
  static uint8_t brightness = 0;
  static int8_t direction = 1;
  
  brightness += direction * 2;
  
  if (brightness >= 255 || brightness <= 0) {
    direction = -direction;
  }
  
  leds[0] = CRGB::Blue;  // Ganti warna sesuai keinginan
  FastLED.setBrightness(brightness);
  FastLED.show();
  delay(20);
}
```

#### **Efek Blinking (Berkedip on-off):**
```cpp
void loop() {
  // ON
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  
  // OFF
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
}
```

#### **Efek Chasing (Untuk strip LED):**
```cpp
void loop() {
  static uint8_t pos = 0;
  
  // Clear all
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  
  // Light one LED
  leds[pos] = CRGB::Red;
  FastLED.show();
  
  pos = (pos + 1) % NUM_LEDS;
  delay(100);
}
```

---

## 📊 Spesifikasi WS2812B

### Karakteristik LED:
| Parameter | Nilai | Keterangan |
|-----------|-------|------------|
| **Voltage** | 5V DC | 3.5-5.5V range |
| **Arus per LED** | 60mA | Max (saat putih full brightness) |
| **Power per LED** | 0.3W | Saat full brightness |
| **Protocol** | Digital | One-wire data |
| **Colors** | 16.7 million | RGB 24-bit |
| **Refresh rate** | 400Hz | 2.5ms per update |

### Power Consumption:
| Jumlah LED | Arus (maks) | Power Supply |
|------------|-------------|--------------|
| 1 LED | 60 mA | USB ESP32 OK |
| 5 LED | 300 mA | USB ESP32 OK |
| 10 LED | 600 mA | USB limit! |
| 30 LED | 1.8 A | Pakai PSU 5V/2A |
| 60 LED | 3.6 A | Pakai PSU 5V/5A |

**⚠️ Catatan Power:**
- USB biasanya limit 500mA
- Jika >5 LED, gunakan power supply eksternal
- Sambungkan GND PSU ke GND ESP32

---

## 🎓 Belajar Lebih Lanjut

### Memahami HSV Color Model:

Program ini menggunakan **HSV** (Hue, Saturation, Value) untuk rainbow:

```cpp
leds[0] = CHSV(hue, 255, 255);
//             |    |    |
//             |    |    +-- Value (brightness): 0-255
//             |    +------- Saturation (color intensity): 0-255
//             +------------ Hue (color): 0-255 (0=red, 85=green, 170=blue)
```

**Hue Values:**
- 0 = Merah
- 32 = Oranye
- 64 = Kuning
- 96 = Hijau
- 128 = Cyan
- 160 = Biru
- 192 = Ungu
- 224 = Magenta
- 255 = Merah (kembali ke awal)

---

## 🚀 Project Ideas

Setelah berhasil, coba project ini:

1. **LED Indicator Status**
   - Merah = Error
   - Kuning = Warning
   - Hijau = OK
   - Biru = Processing

2. **Music Visualizer**
   - LED bereaksi terhadap suara
   - Gunakan microphone + FFT

3. **Notification Light**
   - LED berkedip saat ada notifikasi
   - WiFi/Bluetooth connected

4. **LED Strip Effects**
   - Running light
   - Fire effect
   - Sparkle
   - Theater chase

5. **Smart Home Indicator**
   - Status sensor (temperature, humidity)
   - Door open/close indicator
   - Motion detection alert

---

## 📚 Referensi & Resources

### Library & Documentation:
- **FastLED Library**: https://github.com/FastLED/FastLED
- **FastLED Wiki**: https://github.com/FastLED/FastLED/wiki
- **WS2812B Datasheet**: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf

### Tutorial:
- **FastLED Examples**: File → Examples → FastLED (di Arduino IDE)
- **Color Palettes**: https://github.com/FastLED/FastLED/wiki/Gradient-color-palettes

---

## 📝 Changelog

**v1.0** (Current)
- ✅ Basic color test (RGBW)
- ✅ Rainbow cycle effect
- ✅ Serial Monitor feedback
- ✅ Safe brightness setting (80/255)
- ✅ Support single LED & LED strip

---

## ⚖️ License

Program ini gratis untuk digunakan untuk keperluan edukasi dan development.

---

**Selamat mencoba! Semoga LED-nya nyala! 🌈✨**

*Jika program berjalan dengan baik, Anda siap untuk membuat project LED yang lebih kompleks seperti LED matrix, VU meter, atau ambient lighting!*

copyright by hwthinker
