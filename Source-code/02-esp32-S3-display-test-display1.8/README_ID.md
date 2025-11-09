# ESP32-S3 Display Test - ST7735 1.8 inch

Proyek Arduino untuk menguji dan menampilkan grafis pada display ST7735 1.8 inch (128x160) menggunakan ESP32-S3.

## 📋 Deskripsi

Sketch Arduino ini mendemonstrasikan cara mengontrol display TFT ST7735 1.8 inch dengan ESP32-S3. Program menampilkan teks "Hello ESP32-S3!" dengan berbagai warna dan animasi sederhana untuk memverifikasi fungsi display.

## 🛠️ Hardware yang Dibutuhkan

- **Microcontroller**: ESP32-S3
- **Display**: ST7735 1.8 inch TFT (128x160 pixels)
- **Library**: Adafruit GFX + Adafruit ST7735 and ST7789

## 🔌 Konfigurasi Pin

| Pin Display | Pin ESP32-S3 | Fungsi |
|-------------|--------------|---------|
| SCLK | GPIO 21 | Clock SPI |
| MOSI | GPIO 47 | Data SPI |
| CS | GPIO 41 | Chip Select |
| DC | GPIO 40 | Data/Command |
| RST | GPIO 45 | Reset |
| BLK | GPIO 42 | Backlight Control |
| VCC | 3.3V | Power |
| GND | GND | Ground |

## 📦 Library yang Diperlukan

1. Adafruit GFX Library - Graphics library untuk display
2. Adafruit ST7735 and ST7789 Library - Driver untuk display ST7735
3. SPI Library - Komunikasi SPI (sudah termasuk dalam Arduino)

### Instalasi Library

1. Buka Arduino IDE
2. Pergi ke **Tools** → **Manage Libraries**
3. Cari dan instal:
   - "Adafruit GFX Library"
   - "Adafruit ST7735 and ST7789 Library"

## 🚀 Cara Menggunakan

### 1. Wiring Hardware

Hubungkan display ST7735 ke ESP32-S3 sesuai konfigurasi pin di atas. Pastikan koneksi VCC (3.3V) dan GND sudah benar.

### 2. Upload Kode

1. Buka file `02-esp32-S3-display-test-display1.8.ino` di Arduino IDE
2. Pilih board: ESP32S3 Dev Module
3. Pilih port COM yang sesuai
4. Upload kode ke ESP32-S3

### 3. Hasil yang Diharapkan

Setelah upload berhasil, display akan menampilkan:
- Border biru di sekeliling layar
- Teks "Hello" berwarna kuning
- Teks "ESP32-S3!" berwarna hijau
- Baris titik berwarna merah dan cyan di bagian bawah
- Titik berkedip merah/putih di pojok kanan atas

## ⚙️ Konfigurasi Display

### Inisialisasi Tab

Kode menggunakan `INITR_BLACKTAB` untuk inisialisasi. Jika warna atau offset tampak aneh, coba ganti dengan:

```cpp
// Opsi alternatif:
tft.initR(INITR_GREENTAB);   // Untuk modul dengan tab hijau
tft.initR(INITR_REDTAB);     // Untuk modul dengan tab merah
```

### Orientasi Display

Ubah orientasi display dengan mengubah parameter `setRotation()`:

```cpp
tft.setRotation(0);  // 0-3: 0=portrait, 1=landscape, 2=portrait terbalik, 3=landscape terbalik
```

### Kecepatan SPI

Kecepatan SPI default diatur ke 20 MHz untuk stabilitas:

```cpp
tft.setSPISpeed(20000000); // 20 MHz
```

## 📝 Penjelasan Kode

### Fungsi Utama

- `backlightOn()`: Mengaktifkan backlight display
- `setup()`: Inisialisasi SPI, display, dan menampilkan konten awal
- `loop()`: Animasi titik berkedip di pojok kanan atas

### Fitur Grafis yang Ditampilkan

1. **Background**: Hitam (`ST77XX_BLACK`)
2. **Border**: Biru (`ST77XX_BLUE`)
3. **Teks**: "Hello" (kuning) dan "ESP32-S3!" (hijau)
4. **Titik-titik**: Baris titik berwarna merah dan cyan
5. **Animasi**: Titik berkedip merah/putih

## 🔧 Troubleshooting

### Display Tidak Menyala
- Periksa koneksi VCC (3.3V) dan GND
- Pastikan pin backlight (GPIO 42) terhubung dan aktif HIGH

### Warna Tidak Sesuai
- Coba ganti `INITR_BLACKTAB` dengan `INITR_GREENTAB` atau `INITR_REDTAB`
- Periksa koneksi pin DC dan RST

### Display Blank
- Pastikan library Adafruit ST7735 terinstal
- Periksa koneksi pin CS, DC, dan RST
- Verifikasi konfigurasi pin di kode sesuai wiring

### SPI Error
- Pastikan pin SCLK (21) dan MOSI (47) terhubung dengan benar
- Coba turunkan kecepatan SPI: `tft.setSPISpeed(10000000)` // 10 MHz

## 📁 Struktur File

```
02-esp32-S3-display-test-display1.8/
├── 02-esp32-S3-display-test-display1.8.ino  # File sketch utama
└── README.md                                # Dokumentasi ini
```

## 🤝 Kontribusi

Silakan fork repository ini dan buat pull request untuk perbaikan atau fitur tambahan.

## 📄 Lisensi

Proyek ini menggunakan lisensi MIT. Lihat file LICENSE untuk detail lengkap.

## 📞 Support

Jika mengalami masalah, buat issue di repository GitHub atau hubungi melalui email.

---

**Dibuat dengan ❤️ untuk komunitas ESP32 dan Arduino**