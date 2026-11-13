#pragma once
#include <Arduino.h>

struct SensorData {
  float soil_pct = NAN;   // 0..100
  bool  soil_ok  = false; // false = cam bien dat loi
  float temp     = NAN;   // do C
  float hum      = NAN;   // % khong khi
  bool  dht_ok   = false;
  bool  water_ok = true;  // false = het nuoc
};

void sensor_begin();
void sensor_read(SensorData &out);

// Ham thuan, de test tren may: ADC -> %
float soil_raw_to_pct(int raw);
bool  soil_raw_valid(int raw);
