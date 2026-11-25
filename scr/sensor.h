#pragma once
#include <Arduino.h>

struct SensorData {
  float soil_pct = NAN;   // 0..100
  bool  soil_ok  = false; // false = cam bien dat loi
  float temp     = NAN;   // DHT22 chua duoc doc
  float hum      = NAN;   // DHT22 chua duoc doc
  bool  dht_ok   = false; // DHT22 chua duoc doc
  bool  water_ok = true;  // tam cho phep bom khi chua doc cam bien muc nuoc
};

void sensor_begin();
void sensor_read(SensorData &out);
bool sensor_set_soil_override(float soil_pct);
void sensor_clear_soil_override();

// Ham thuan, de test tren may: ADC -> %
float soil_raw_to_pct(int raw);
bool  soil_raw_valid(int raw);
