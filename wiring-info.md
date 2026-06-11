# Wiring Info — Goouuu ESP32-S3 ChatAI Display 1.8 Inch

Microcontroller: **ESP32-S3-WROOM N16R8** (16MB Flash, 8MB OPI PSRAM)

---

## 1. WS2812B RGB LED (NeoPixel)

**Protokol:** 1-Wire (NeoPixel / single-wire digital)

| ESP32-S3 Pin | WS2812B Pin | Keterangan |
|---|---|---|
| GPIO 48 | DI / DIN | Data Input |
| 3V3 (atau 5V) | VCC / VDD | Power (3.3V untuk 1 LED) |
| GND | GND | Ground |

**Library:** Adafruit NeoPixel (direkomendasikan untuk ESP32-S3 dengan I2S)
> **Catatan:** Jangan gunakan FastLED jika memakai I2S (speaker/mic) karena conflict RMT channel → bootloop.

---

## 2. MAX98357A — Digital Amplifier / Speaker

**Protokol:** I2S (Inter-IC Sound) — TX (output audio)

| ESP32-S3 Pin | MAX98357A Pin | Keterangan |
|---|---|---|
| GPIO 7 | DIN | Digital Audio Data |
| GPIO 15 | BCLK | Bit Clock |
| GPIO 16 | LRC | Left/Right Clock (Word Select) |
| 3V3 | Vin / VCC | Power 3.3V |
| GND | GND | Ground (short connect GAIN untuk mono) |
| — | Audio+ | Ke speaker (kabel merah) |
| — | Audio- | Ke speaker negatif |

**I2S Mode:** MASTER TX, 44100 Hz, 16-bit stereo

---

## 3. INMP441 — Digital Microphone

**Protokol:** I2S (Inter-IC Sound) — RX (input audio)

| ESP32-S3 Pin | INMP441 Pin | Keterangan |
|---|---|---|
| GPIO 4 | WS | Word Select / Data Select |
| GPIO 5 | SCK | Serial Clock / Data Clock |
| GPIO 6 | SD | Serial Data Output |
| 3V3 | VDD | Power 3.3V |
| GND | GND | Ground (short connect L/R untuk left channel) |

**I2S Mode:** MASTER RX, 16000 Hz, 32-bit, left channel only

---

## 4. IPS Display ST7735 1.8" (128×160)

**Protokol:** SPI (Serial Peripheral Interface) — menggunakan FSPI bus ESP32-S3

| ESP32-S3 Pin | Display Pin | Keterangan |
|---|---|---|
| GPIO 21 | SCL | SPI Clock |
| GPIO 47 | SDA | SPI Data (MOSI) |
| GPIO 45 | RES | Reset Signal |
| GPIO 40 | DC | Data/Command Select |
| GPIO 41 | CS | Chip Select |
| GPIO 42 | BLK | Backlight Control (HIGH = ON) |
| 3V3 | VCC | Power 3.3V |
| GND | GND | Ground |

**Driver IC:** ST7735S  
**Library:** Adafruit ST7735 + Adafruit GFX  
**SPI Speed:** 20 MHz  
**MISO:** tidak dipakai (-1)

---

## 5. USB Serial — Programming & Debug

**Protokol:** UART via USB

| Mode | Chip | Keterangan |
|---|---|---|
| USB Serial Converter (default) | CH343 | Switch hardware ke posisi Serial USB, USB CDC on Boot = Disabled |
| Native USB | ESP32-S3 built-in | Switch hardware ke posisi Native USB, USB CDC on Boot = Enabled |

**Baud rate upload:** 921600  
**Baud rate serial monitor:** 115200 (untuk test code)  

---

## Ringkasan GPIO Map

| GPIO | Device | Fungsi | Protokol |
|---|---|---|---|
| 4 | INMP441 Mic | WS (Word Select) | I2S RX |
| 5 | INMP441 Mic | SCK (Clock) | I2S RX |
| 6 | INMP441 Mic | SD (Data) | I2S RX |
| 7 | MAX98357A Speaker | DIN (Data) | I2S TX |
| 15 | MAX98357A Speaker | BCLK (Clock) | I2S TX |
| 16 | MAX98357A Speaker | LRC (Word Select) | I2S TX |
| 21 | ST7735 Display | SCL (SPI Clock) | SPI |
| 40 | ST7735 Display | DC (Data/Command) | SPI |
| 41 | ST7735 Display | CS (Chip Select) | SPI |
| 42 | ST7735 Display | BLK (Backlight) | GPIO OUT |
| 45 | ST7735 Display | RES (Reset) | GPIO OUT |
| 47 | ST7735 Display | SDA (MOSI) | SPI |
| 48 | WS2812B LED | DIN (Data) | 1-Wire NeoPixel |

---

## Catatan Tambahan

- Tombol konfigurasi WiFi terhubung ke **GPIO 1** (tekan + RST untuk masuk WiFi config mode)
- LED RGB berfungsi sebagai indikator status: biru kedip = WiFi config, hijau = terhubung, merah = rekam audio
- Firmware XiaoZhi menggunakan koneksi **WiFi 2.4 GHz** untuk cloud AI
