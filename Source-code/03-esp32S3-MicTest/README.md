# 📱 ESP32-S3 Microphone Test - Panduan Lengkap untuk Pemula

## 📋 Daftar Isi
- [Tentang Program](#-tentang-program)
- [Hardware yang Dibutuhkan](#-hardware-yang-dibutuhkan)
- [Koneksi Hardware](#-koneksi-hardware)
- [Instalasi Software](#-instalasi-software)
- [Cara Upload Program](#-cara-upload-program)
- [Cara Menggunakan](#-cara-menggunakan)
- [Troubleshooting](#-troubleshooting)

---

## 🎯 Tentang Program

Program ini adalah **tes microphone INMP441** untuk ESP32-S3 yang membantu Anda:
- ✅ Mengecek apakah microphone terpasang dengan benar
- ✅ Mengukur level suara ambient (baseline)
- ✅ Mendeteksi suara (tepuk tangan, bicara, ketuk)
- ✅ Menampilkan visualisasi bar graph di Serial Monitor

**Fitur Utama:**
- 📊 Baseline noise measurement otomatis
- 🎤 Real-time sound detection
- 📈 Visual bar graph (30 karakter)
- 🚨 Warning untuk koneksi yang salah
- 🔴 Indikator emoji untuk level suara

---

## 🛠️ Hardware yang Dibutuhkan

1. ESP32-S3 Board
2. Shied chat AI V2 (INMP441 I2S Microphone integrated)
3. Speaker 
4. Kabel USB



## 🔌 Koneksi Hardware

### Diagram Koneksi INMP441 ke ESP32-S3

```
INMP441 Pin    →    ESP32-S3 Pin
─────────────────────────────────
VDD (Power)    →    3.3V
GND (Ground)   →    GND
WS  (Word Sel) →    GPIO 4
SCK (Clock)    →    GPIO 5
SD  (Data)     →    GPIO 6
L/R (Channel)  →    GND  ⚠️ PENTING!
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

### Langkah 3: Tidak Ada Library Tambahan!
✅ Program ini **TIDAK** memerlukan library tambahan!
- Hanya menggunakan library bawaan ESP32 (`driver/i2s.h`)

---

## ⬆️ Cara Upload Program

### Langkah 1: Buka File Program
1. Buka file **`03-esp32S3-MicTest.ino`** di Arduino IDE
2. Atau copy-paste kode ke sketch baru

### Langkah 2: Pilih Board
1. Klik **Tools** → **Board** → **ESP32 Arduino**
2. Pilih **ESP32S3 Dev Module**

### Langkah 3: Konfigurasi Board (Penting!)
Set konfigurasi seperti ini:
```
Tools →
  ├─ USB CDC On Boot:      "Enabled"
  ├─ CPU Frequency:        "240MHz (WiFi)"
  ├─ Flash Mode:           "QIO 80MHz"
  ├─ Flash Size:           "16MB (128Mb)" (sesuaikan board Anda)
  ├─ Partition Scheme:     "Huge APP (3MB No OTA)"
  ├─ PSRAM:                "OPI PSRAM"  ⚠️ Atau "Disabled" jika tidak punya
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

### Langkah 2: Proses Kalibrasi (5 Detik)
Program akan menjalankan kalibrasi otomatis:

```
╔════════════════════════════════════════════╗
║   INMP441 DEBUG - Detect Sound Changes    ║
╚════════════════════════════════════════════╝

Warming up.......... Done!

📊 Measuring baseline noise...
🤫 STAY QUIET for 5 seconds!

..... Done!

✓ Baseline noise level: 245
✓ Detection threshold: 345 (baseline + 100)
```

**⚠️ PENTING saat kalibrasi:**
- 🤫 **JANGAN MEMBUAT SUARA** selama 5 detik
- Ini untuk mengukur noise ambient ruangan
- Hasil baseline yang bagus: **100-500**

### Langkah 3: Test Microphone!
Setelah kalibrasi, mulai tes dengan:
- 👏 **Tepuk tangan**
- 🗣️ **Bicara** ke microphone
- 🔨 **Ketuk meja** atau permukaan

### Langkah 4: Baca Output Serial Monitor

```
[████████████              ] Lvl: 520 Base: 245 Diff:+275 P2P: 1240 🔴 SOUND DETECTED!
[█████                     ] Lvl: 180 Base: 245 Diff: -65 P2P:  450 ⚪ Ambient
[███████████████           ] Lvl: 680 Base: 245 Diff:+435 P2P: 1850 🔴 SOUND DETECTED!
```

**Penjelasan Output:**
- **Bar `[████]`**: Visualisasi level suara (30 karakter)
- **Lvl**: Level suara saat ini (Average)
- **Base**: Baseline ambient noise
- **Diff**: Perbedaan dari baseline (+/-)
- **P2P**: Peak-to-Peak (range amplitudo)

**Indikator Emoji:**
- 🔴 **SOUND DETECTED!** → Diff > 150 (suara terdeteksi jelas)
- 🟡 **Small change** → Diff > 80 (perubahan kecil)
- 🔵 **Below baseline** → Diff < -80 (di bawah baseline)
- ⚪ **Ambient** → Diff normal (noise ruangan)

---

## 🔧 Troubleshooting

### ❌ Problem: Baseline terlalu rendah (< 50)

```
⚠️  WARNING: Baseline is TOO LOW!
   This usually means:
   - L/R pin is NOT grounded properly
   - Mic is not receiving power
   - Mic is faulty
```

**Solusi:**
1. ✅ Pastikan pin **L/R** INMP441 terhubung ke **GND**
2. ✅ Cek koneksi **VDD** ke **3.3V** (jangan 5V!)
3. ✅ Cek koneksi **GND** sudah benar
4. 🔄 Reset ESP32-S3 (tekan tombol RST)

---

### ❌ Problem: Baseline terlalu tinggi (> 800)

```
⚠️  WARNING: Baseline is TOO HIGH!
   This usually means:
   - Too much electrical noise
   - L/R pin is floating (not grounded)
   - Board has interference
```

**Solusi:**
1. ✅ Pastikan pin **L/R** terhubung ke **GND** (bukan floating)
2. ✅ Coba kabel jumper yang lebih pendek
3. ✅ Jauhkan dari sumber noise (motor, WiFi router)
4. 🔌 Gunakan power supply yang stabil

---

### ❌ Problem: Tidak ada respon saat bicara/tepuk

**Kemungkinan Penyebab:**
1. **Koneksi WS, SCK, SD salah**
   - Cek lagi GPIO 4, 5, 6
   - Pastikan tidak tertukar

2. **INMP441 rusak**
   - Coba microphone lain jika ada

3. **Baseline tidak stabil**
   - Ulangi upload & kalibrasi
   - Pastikan ruangan tenang saat kalibrasi

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

## 📊 Nilai Baseline yang Bagus

| Baseline | Status | Arti |
|----------|--------|------|
| < 50 | ❌ Terlalu Rendah | Mic tidak terhubung benar |
| 50-100 | ⚠️ Rendah | Cek koneksi L/R ke GND |
| 100-500 | ✅ **BAGUS** | Mic berfungsi normal |
| 500-800 | ⚠️ Tinggi | Ada noise, tapi masih bisa |
| > 800 | ❌ Terlalu Tinggi | Terlalu banyak noise/error |

---

## 🎯 Tips & Trik

### ✅ Untuk Hasil Terbaik:
1. **Kalibrasi di ruangan tenang**
   - Matikan kipas, AC, atau peralatan berisik
   - Lakukan di malam hari jika perlu

2. **Jarak microphone ke sumber suara**
   - Tepuk tangan: 10-30 cm
   - Bicara: 5-15 cm
   - Ketuk meja: microphone di atas meja

3. **Kabel jumper pendek**
   - Gunakan kabel maksimal 15cm
   - Kabel terlalu panjang → noise tinggi

4. **Power supply stabil**
   - Gunakan USB port langsung dari komputer
   - Hindari USB hub murah

---

## 🎓 Untuk Belajar Lebih Lanjut

### Memahami Parameter:

**1. Lvl (Level):**
- Rata-rata amplitudo absolut
- Semakin keras suara → nilai semakin tinggi
- Range: 0 - 2000+ (tergantung suara)

**2. Base (Baseline):**
- Noise ambient saat kalibrasi
- Nilai referensi untuk deteksi suara
- Diukur saat ruangan tenang

**3. Diff (Difference):**
- Selisih antara Lvl dan Base
- Positif (+) → Ada suara
- Negatif (-) → Di bawah baseline
- Threshold deteksi: +150

**4. P2P (Peak-to-Peak):**
- Jarak antara sample tertinggi & terendah
- Indikator dinamika suara
- Semakin besar → suara semakin keras/dinamis

---

## 📚 Referensi & Resources

- **INMP441 Datasheet**: https://invensense.tdk.com/products/digital/inmp441/
- **ESP32-S3 I2S Guide**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/i2s.html
- **Arduino ESP32**: https://github.com/espressif/arduino-esp32

---

## 📝 Changelog

**v1.0** (Current)
- ✅ Baseline measurement otomatis
- ✅ Real-time sound detection
- ✅ Visual bar graph
- ✅ Warning system untuk koneksi salah
- ✅ Emoji indicator

---

## ⚖️ License

Program ini gratis untuk digunakan untuk keperluan edukasi dan development.

---

**Selamat mencoba! Semoga berhasil! 🚀**

*Jika program berjalan dengan baik, Anda siap untuk project audio berikutnya seperti voice recorder, voice command, atau noise detection!*
