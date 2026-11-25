#include "sensor.h"
#include "config.h"
// #include <DHT.h>

// static DHT dht(PIN_DHT, DHT22);
static bool  g_soil_override_enabled = false;
static float g_soil_override_pct = 0.0f;

void sensor_begin() {
  analogReadResolution(12);
  // pinMode(PIN_WATER, INPUT_PULLUP);
  // dht.begin();
}

bool soil_raw_valid(int raw) {
  // ADC kep o 2 dau = day dut / chap mach
  return raw > 50 && raw < 4090;
}

float soil_raw_to_pct(int raw) {
  float pct = (float)(SOIL_ADC_DRY - raw) * 100.0f / (float)(SOIL_ADC_DRY - SOIL_ADC_WET);
  return constrain(pct, 0.0f, 100.0f);
}

bool sensor_set_soil_override(float soil_pct) {
  if (!(soil_pct >= 0.0f && soil_pct <= 100.0f)) return false;
  g_soil_override_pct = soil_pct;
  g_soil_override_enabled = true;
  return true;
}

void sensor_clear_soil_override() {
  g_soil_override_enabled = false;
}

void sensor_read(SensorData &out) {
  if (g_soil_override_enabled) {
    out.soil_pct = g_soil_override_pct;
    out.soil_ok = true;
  } else {
#if SOIL_SIMULATION
    const uint32_t phase = millis() % SOIL_SIMULATION_PERIOD_MS;
    const float progress = (phase < SOIL_SIMULATION_PERIOD_MS / 2)
        ? (float)phase / (SOIL_SIMULATION_PERIOD_MS / 2)
        : (float)(SOIL_SIMULATION_PERIOD_MS - phase) / (SOIL_SIMULATION_PERIOD_MS / 2);
    out.soil_pct = SOIL_SIMULATION_MIN_PCT
        + progress * (SOIL_SIMULATION_MAX_PCT - SOIL_SIMULATION_MIN_PCT);
    out.soil_ok = true;
#else
    // Do am dat: trung binh 16 mau
    long sum = 0;
    for (int i = 0; i < 16; i++) { sum += analogRead(PIN_SOIL); delayMicroseconds(200); }
    int raw = sum / 16;
    out.soil_ok  = soil_raw_valid(raw);
    out.soil_pct = out.soil_ok ? soil_raw_to_pct(raw) : NAN;
#endif
  }

  // DHT22 va cam bien muc nuoc de danh dau hint, chua doc trong phien ban nay.
  // float h = dht.readHumidity();
  // float t = dht.readTemperature();
  // out.dht_ok = !isnan(h) && !isnan(t);
  // out.hum = out.dht_ok ? h : NAN;
  // out.temp = out.dht_ok ? t : NAN;
  // out.water_ok = (digitalRead(PIN_WATER) == HIGH);
}
