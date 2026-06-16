# Wiring Info — Goouuu ESP32-S3 ChatAI Display 1.8 Inch

Microcontroller: **ESP32-S3-WROOM N16R8** (16MB Flash, 8MB OPI PSRAM)

> Pin assignment di dokumen ini sudah diverifikasi langsung dari seluruh sketch di folder `Source-code/` (00 s.d. 07).

---

## 1. WS2812B RGB LED (NeoPixel)

**Protokol:** 1-Wire (NeoPixel / single-wire digital)

| ESP32-S3 Pin | WS2812B Pin | Keterangan |
|---|---|---|
| GPIO 48 | DI / DIN | Data Input |
| 3V3 (atau 5V) | VCC / VDD | Power (3.3V untuk 1 LED) |
| GND | GND | Ground |

**Library:** Adafruit NeoPixel (direkomendasikan untuk ESP32-S3 dengan I2S)
> **Catatan:** Sketch `00-esp32S3-WS2812BTest` masih memakai FastLED untuk uji LED standalone. Untuk sketch yang juga menjalankan I2S (speaker/mic) → gunakan Adafruit NeoPixel agar tidak konflik RMT/I2S channel.

---

## 2. MAX98357A — Digital Amplifier / Speaker

**Protokol:** I2S (Inter-IC Sound) — TX (output audio), port `I2S_NUM_0`

| ESP32-S3 Pin | MAX98357A Pin | Keterangan |
|---|---|---|
| GPIO 7 | DIN | Digital Audio Data |
| GPIO 15 | BCLK | Bit Clock |
| GPIO 16 | LRC | Left/Right Clock (Word Select) |
| 3V3 | Vin / VCC | Power 3.3V |
| GND | GND | Ground (short ke GAIN untuk mono) |
| — | Audio+ | Ke speaker (kabel merah) |
| — | Audio- | Ke speaker negatif |

**I2S Mode:** MASTER TX, 16-bit stereo
- `01-esp32S3-SpeakerTest` → 44100 Hz tone test
- Sketch 04–07 → 16000 Hz (sinkron dengan mic untuk record/play)

---

## 3. INMP441 — Digital Microphone

**Protokol:** I2S (Inter-IC Sound) — RX (input audio), port `I2S_NUM_1` saat bareng speaker (sketch 04–07), atau `I2S_NUM_0` saat standalone (sketch 03)

| ESP32-S3 Pin | INMP441 Pin | Keterangan |
|---|---|---|
| GPIO 4 | WS | Word Select / Data Select |
| GPIO 5 | SCK | Serial Clock / Data Clock |
| GPIO 6 | SD | Serial Data Output |
| 3V3 | VDD | Power 3.3V |
| GND | GND | Ground (short L/R untuk left channel) |

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

## 5. BOOT Button (GPIO 0) — Trigger Input

Tombol BOOT bawaan board dipakai sebagai input pemicu di sketch 04, 05, 06, 07:
- Sketch 04 → tekan = play beep
- Sketch 05 → tap pendek = beep, tahan ≥1 dtk = ganti LED mode
- Sketch 06 / 07 → tekan = mulai recording

Pada **firmware XiaoZhi**, tombol BOOT (GPIO 0) juga dipakai untuk masuk WiFi config mode (tekan BOOT + RST).

| ESP32-S3 Pin | Mode | Fungsi |
|---|---|---|
| GPIO 0 | INPUT_PULLUP | BOOT button (active LOW) |

> GPIO 0 adalah strapping pin. Aman dipakai sebagai input saat runtime, tapi jangan tahan saat reset/upload (akan masuk download mode).

---

## 6. USB Serial — Programming & Debug

**Protokol:** UART via USB

| Mode | Chip | Keterangan |
|---|---|---|
| USB Serial Converter (default) | CH343 | Switch hardware ke posisi Serial USB, USB CDC on Boot = Disabled |
| Native USB | ESP32-S3 built-in | Switch hardware ke posisi Native USB, USB CDC on Boot = Enabled |

**Baud rate upload:** 921600
**Baud rate serial monitor:** 115200

---

## Ringkasan GPIO Map — Pin Yang DIPAKAI

| GPIO | Device | Fungsi | Protokol | Sketch |
|---|---|---|---|---|
| 0 | BOOT button | Trigger input (active LOW) | GPIO IN | 04, 05, 06, 07 |
| 4 | INMP441 Mic | WS (Word Select) | I2S RX | 03, 04, 05, 06, 07 |
| 5 | INMP441 Mic | SCK (Clock) | I2S RX | 03, 04, 05, 06, 07 |
| 6 | INMP441 Mic | SD (Data) | I2S RX | 03, 04, 05, 06, 07 |
| 7 | MAX98357A Speaker | DIN (Data) | I2S TX | 01, 04, 05, 06, 07 |
| 15 | MAX98357A Speaker | BCLK (Clock) | I2S TX | 01, 04, 05, 06, 07 |
| 16 | MAX98357A Speaker | LRC (Word Select) | I2S TX | 01, 04, 05, 06, 07 |
| 21 | ST7735 Display | SCL (SPI Clock) | SPI (FSPI) | 02, 04, 05, 06, 07 |
| 40 | ST7735 Display | DC (Data/Command) | GPIO OUT | 02, 04, 05, 06, 07 |
| 41 | ST7735 Display | CS (Chip Select) | SPI (FSPI) | 02, 04, 05, 06, 07 |
| 42 | ST7735 Display | BLK (Backlight) | GPIO OUT | 02, 04, 05, 06, 07 |
| 45 | ST7735 Display | RES (Reset) | GPIO OUT | 02, 04, 05, 06, 07 |
| 47 | ST7735 Display | SDA (MOSI) | SPI (FSPI) | 02, 04, 05, 06, 07 |
| 48 | WS2812B LED | DIN (Data) | 1-Wire NeoPixel / RMT | 00, 05, 06, 07 |

Total: **14 GPIO terpakai** (termasuk BOOT button).

---

## GPIO Yang MASIH FREE / Bebas Dipakai

ESP32-S3 punya pin GPIO 0–21 dan 26–48 (GPIO 22–25 tidak ada di chip ESP32-S3). Pada N16R8, GPIO 26–37 sudah otomatis dipakai internal flash + OPI PSRAM, jadi **tidak boleh disentuh**.

### ✅ Bebas dipakai untuk peripheral baru (aman, tanpa caveat)

| GPIO | Catatan |
|---|---|
| 2  | Bebas (input/output digital, ADC1_CH1, touch) |
| 8  | Bebas (input/output digital, ADC1_CH7) |
| 9  | Bebas (input/output digital, ADC1_CH8, touch) |
| 10 | Bebas (input/output digital, ADC1_CH9, touch) |
| 11 | Bebas (input/output digital, ADC2_CH0, touch) |
| 13 | Bebas (input/output digital, ADC2_CH2, touch) |
| 14 | Bebas (input/output digital, ADC2_CH3, touch) |
| 17 | Bebas (input/output digital, ADC2_CH6) |
| 18 | Bebas (input/output digital, ADC2_CH7) |
| 38 | Bebas (input/output digital) |
| 39 | Bebas (input/output digital, MTCK) |

### ⚠️ Bisa dipakai dengan catatan (strapping / fungsi khusus)

| GPIO | Catatan |
|---|---|
| 1  | Bebas di Arduino sketch maupun firmware XiaoZhi. |
| 12 | ADC2_CH1, RTC, FSPICLK, TOUCH12. **Dipakai expansion board** — jangan dipakai ulang sebagai output saat expansion terpasang. |
| 3  | Strapping pin (JTAG select). Aman sebagai input runtime; hindari sebagai output saat boot. |
| 19 | USB D− (Native USB). Free hanya jika USB switch di posisi CH343 dan tidak pakai Native USB. |
| 20 | USB D+ (Native USB). Free hanya jika USB switch di posisi CH343 dan tidak pakai Native USB. |
| 43 | UART0 TX (debug serial). Free jika serial debug tidak diperlukan. |
| 44 | UART0 RX (debug serial). Free jika serial debug tidak diperlukan. |
| 46 | Strapping pin (boot mode). Bisa dipakai sebagai output dengan hati-hati; jangan tarik HIGH saat reset. |

### ❌ Tidak boleh dipakai (terkunci untuk flash & PSRAM N16R8)

| GPIO | Fungsi internal |
|---|---|
| 22–25 | Tidak ada (tidak di-bonded di package ESP32-S3) |
| 26 | SPI Flash CS |
| 27 | SPI Flash D / SPIHD |
| 28 | SPI Flash Q / SPIWP |
| 29 | SPI Flash CLK |
| 30 | SPI Flash D0 / SPID |
| 31 | SPI Flash D1 / SPIQ |
| 32 | SPI Flash HD / SPIHD |
| 33 | OPI PSRAM (SPIIO4) |
| 34 | OPI PSRAM (SPIIO5) |
| 35 | OPI PSRAM (SPIIO6) |
| 36 | OPI PSRAM (SPIIO7) |
| 37 | OPI PSRAM (SPIDQS) |

---

## Ringkasan Cepat

- **Pin terpakai:** 0, 4, 5, 6, 7, 15, 16, 21, 40, 41, 42, 45, 47, 48 → **14 pin**
- **Pin free aman:** 2, 8, 9, 10, 11, 13, 14, 17, 18, 38, 39 → **11 pin**
- **Pin free bersyarat:** 1, 3, 12, 19, 20, 43, 44, 46 → **8 pin**
- **Pin terkunci PSRAM/Flash:** 26–37 → **tidak boleh disentuh**

---

## Catatan Tambahan

- Tombol BOOT (GPIO 0) dipakai sebagai trigger di sketch 04–07. Jika ingin menambah tombol lain, gunakan GPIO 2/8/9/10 dengan `INPUT_PULLUP`.
- LED RGB (WS2812B di GPIO 48) berfungsi sebagai indikator status: kuning = wait, merah = recording, hijau = playing, off = idle.
- Firmware XiaoZhi (folder `fimware 1.8 inch`) menggunakan koneksi **WiFi 2.4 GHz** untuk cloud AI. Untuk masuk WiFi config mode: **tekan tombol BOOT (GPIO 0) + RST** (diverifikasi langsung dari hardware — bukan GPIO 1 seperti dokumentasi lama).
- I2S speaker dan mic boleh share port `I2S_NUM_0` (mode standalone), tapi saat dipakai bersamaan harus pisah: speaker `I2S_NUM_0`, mic `I2S_NUM_1` (lihat sketch 04–07).
