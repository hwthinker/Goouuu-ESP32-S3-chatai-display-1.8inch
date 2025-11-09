#include <FastLED.h>

#define LED_DATA_PIN  48  
#define NUM_LEDS      1
#define LED_TYPE      WS2812
#define COLOR_ORDER   GRB

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║   WS2812B LED Safe Test           ║");
  Serial.println("╚════════════════════════════════════╝");
  Serial.printf("Using GPIO %d for LED data\n\n", LED_DATA_PIN);
  
  // Initialize LED
  delay(100);
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(80);
  FastLED.clear();
  FastLED.show();
  
  Serial.println("Testing LED sequence...\n");
  
  // Test Red
  Serial.println("🔴 RED");
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(1000);
  
  // Test Green
  Serial.println("🟢 GREEN");
  leds[0] = CRGB::Green;
  FastLED.show();
  delay(1000);
  
  // Test Blue
  Serial.println("🔵 BLUE");
  leds[0] = CRGB::Blue;
  FastLED.show();
  delay(1000);
  
  // Test White
  Serial.println("⚪ WHITE");
  leds[0] = CRGB::White;
  FastLED.show();
  delay(1000);
  
  // Off
  Serial.println("⚫ OFF");
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  
  Serial.println("\n✅ LED Test Complete!");
  Serial.println("Starting rainbow cycle...\n");
}

void loop() {
  static uint8_t hue = 0;
  
  // Rainbow cycle
  hue += 2;
  leds[0] = CHSV(hue, 255, 255);
  FastLED.show();
  
  // Print to serial every 1 second
  static uint32_t last_print = 0;
  if (millis() - last_print > 1000) {
    Serial.printf("🌈 Rainbow - Hue: %d\n", hue);
    last_print = millis();
  }
  
  delay(20);
}
