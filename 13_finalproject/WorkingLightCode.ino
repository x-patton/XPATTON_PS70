#include <Arduino.h>

// -------- PINS --------
#define MOSFET_PIN   D0    // MOSFET gate
#define SENSOR_PIN   D1    // Phototransistor analog input (ADC)

// -------- LEDC / PWM SETTINGS --------
const uint32_t PWM_FREQ      = 5000;  // 5 kHz
const uint8_t  PWM_RES_BITS  = 8;     // 8-bit resolution → 0–255
const uint16_t PWM_MAX_DUTY  = (1 << PWM_RES_BITS) - 1; // 255

// Lamp brightness limits (in PWM duty units)
const uint16_t MIN_BRIGHTNESS = 10;          // never fully off
const uint16_t MAX_BRIGHTNESS = PWM_MAX_DUTY; // full power

// Update interval
const unsigned long UPDATE_INTERVAL = 200;  // ms
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_PIN, INPUT);

  // New unified LEDC API: attach pin, freq, and resolution in one go
  bool ok = ledcAttach(MOSFET_PIN, PWM_FREQ, PWM_RES_BITS);
  if (!ok) {
    Serial.println("LEDC attach FAILED on MOSFET_PIN!");
  }

  // Start at full brightness
  ledcWrite(MOSFET_PIN, MAX_BRIGHTNESS);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = currentTime;

    // ESP32C3 ADC is 12-bit by default: 0–4095
    int sensorValue = analogRead(SENSOR_PIN);
    Serial.print("Light sensor: ");
    Serial.println(sensorValue);

    // Dark (0)   → bright lamp (MAX_BRIGHTNESS)
    // Bright (4095) → dim lamp (MIN_BRIGHTNESS)
    int brightness = map(sensorValue,
                         0, 4095,
                         MAX_BRIGHTNESS, MIN_BRIGHTNESS);

    brightness = constrain(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);

    // New API uses the *pin* directly
    ledcWrite(MOSFET_PIN, brightness);
  }
}