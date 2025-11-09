// HelloWorld ST7735 1.8" (SPI) – ESP32-S3
// Library: Adafruit GFX + Adafruit ST7735 and ST7789
// Pin mapping (sesuai tabel kamu):
// SCLK=GPIO21, MOSI=GPIO47, CS=41, DC=40, RST=45, BLK=42
//
// Catatan: Banyak modul 1.8" 128x160 = ST7735(S).
// Coba INITR_BLACKTAB dulu; jika warna/offset aneh, ganti GREENTAB/REDTAB.

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// ---- PIN KONFIG ----
#define PIN_TFT_SCLK 21    // SCL
#define PIN_TFT_MOSI 47    // SDA / MOSI
#define PIN_TFT_MISO -1    // tidak dipakai
#define PIN_TFT_CS   41
#define PIN_TFT_DC   40
#define PIN_TFT_RST  45
#define PIN_TFT_BL   42    // backlight (HIGH = ON)

// Resolusi ST7735 1.8"
#define TFT_WIDTH   128
#define TFT_HEIGHT  160

// Pakai bus FSPI (ESP32-S3)
SPIClass spi(FSPI);

// Konstruktor ST7735 pakai SPI custom
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

void backlightOn() {
  pinMode(PIN_TFT_BL, OUTPUT);
  digitalWrite(PIN_TFT_BL, HIGH);  // nyalakan backlight
}

void setup() {
  // Mulai SPI dengan pin custom (SCLK, MISO, MOSI, SS)
  spi.begin(PIN_TFT_SCLK, PIN_TFT_MISO, PIN_TFT_MOSI, PIN_TFT_CS);

  backlightOn();

  // Inisialisasi panel ST7735.
  // Coba salah satu: INITR_BLACKTAB (umum), atau GREENTAB/REDTAB jika warna/offset aneh.
  tft.initR(INITR_BLACKTAB);
  // (opsional) turunkan clock dulu biar stabil saat awal uji
  tft.setSPISpeed(20000000); // 20 MHz

  // Atur orientasi (0..3)
  tft.setRotation(0);

  // Layar uji
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ST77XX_BLUE);

  tft.setTextWrap(false);

  tft.setCursor(8, 60);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.print("Hello");

  tft.setCursor(8, 84);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.print("ESP32-S3!");

  // titik demo biar kelihatan refresh
  for (int i = 0; i < 10; i++) {
    tft.fillCircle(8 + i * 12, 140, 4, (i % 2) ? ST77XX_RED : ST77XX_CYAN);
  }
}

void loop() {
  static uint16_t c = ST77XX_RED;
  static bool toggle = false;
  tft.fillCircle(118, 10, 6, c);
  c = toggle ? ST77XX_RED : ST77XX_WHITE;
  toggle = !toggle;
  delay(400);
}
