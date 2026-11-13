#include "sensor.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT22);

void sensor_begin() {
  analogReadResolution(12);
  pinMode(PIN_WATER, INPUT_PULLUP);
  dht.begin();
}

bool soil_raw_valid(int raw) {
  // ADC kep o 2 dau = day dut / chap mach
  return raw > 50 && raw < 4090;
}

float soil_raw_to_pct(int raw) {
  float pct = (float)(SOIL_ADC_DRY - raw) * 100.0f / (float)(SOIL_ADC_DRY - SOIL_ADC_WET);
  return constrain(pct, 0.0f, 100.0f);
}

void sensor_read(SensorData &out) {
  // Do am dat: trung binh 16 mau
  long sum = 0;
  for (int i = 0; i < 16; i++) { sum += analogRead(PIN_SOIL); delayMicroseconds(200); }
  int raw = sum / 16;
  out.soil_ok  = soil_raw_valid(raw);
  out.soil_pct = out.soil_ok ? soil_raw_to_pct(raw) : NAN;

  // DHT22 (goi cach nhau >= 2s, do app dam bao)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  out.dht_ok = !isnan(h) && !isnan(t);
  out.hum  = out.dht_ok ? h : NAN;
  out.temp = out.dht_ok ? t : NAN;

  out.water_ok = (digitalRead(PIN_WATER) == HIGH);
}
