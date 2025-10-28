#include <Adafruit_NeoPixel.h>

#define LED_PIN     D0     // Pin for LED
#define NUM_PIXELS  5     // Number of LEDs
#define SENSOR_PIN  D1     // Phototransistor pin

// Base color for warm white
const uint8_t BASE_R = 255;
const uint8_t BASE_G = 200;
const uint8_t BASE_B = 120;

const int MIN_BRIGHTNESS = 10;
const int MAX_BRIGHTNESS = 255;

const unsigned long UPDATE_INTERVAL = 200;

class LightSensor {
private:
  int pin;

public:
  LightSensor(int p) : pin(p) {}

  void begin() {
    pinMode(pin, INPUT);
  }

  int read() {
    return analogRead(pin);
  }
};


class GrowLight {
private:
  Adafruit_NeoPixel strip;
  uint8_t baseR, baseG, baseB;
  int minBrightness, maxBrightness;

public:
  GrowLight(uint8_t r, uint8_t g, uint8_t b, int minB, int maxB)
    : strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800),
      baseR(r), baseG(g), baseB(b),
      minBrightness(minB), maxBrightness(maxB) {}

  void begin() {
    strip.begin();
    strip.show(); // Initialize all LEDs to off
  }

  void updateBrightness(int sensorValue) {
    int brightness = map(sensorValue, 0, 4095, maxBrightness, minBrightness);
    brightness = constrain(brightness, minBrightness, maxBrightness);

    uint8_t r = (baseR * brightness) / 255;
    uint8_t g = (baseG * brightness) / 255;
    uint8_t b = (baseB * brightness) / 255;

    // Apply to all LEDs
    for (int i = 0; i < NUM_PIXELS; i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
  }
};


LightSensor sensor(SENSOR_PIN);
GrowLight growlight(BASE_R, BASE_G, BASE_B, MIN_BRIGHTNESS, MAX_BRIGHTNESS);

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  sensor.begin();
  growlight.begin();
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = currentTime;

    int sensorValue = sensor.read();
    Serial.print("Light Sensor: ");
    Serial.println(sensorValue);

    growlight.updateBrightness(sensorValue);
  }
}
