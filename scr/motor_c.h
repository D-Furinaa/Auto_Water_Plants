#pragma once
#include <Arduino.h>
#include "sensor.h"

enum Mode : uint8_t { MODE_AUTO = 0, MODE_MANUAL = 1 };

// Bit canh bao
enum AlertBit : uint8_t {
  ALERT_SENSOR_FAIL  = 1 << 0,
  ALERT_NO_WATER     = 1 << 1,
  ALERT_PUMP_TIMEOUT = 1 << 2,
};

struct Control {
  Mode    mode       = MODE_AUTO;
  float   low        = 30.0f;
  float   high       = 60.0f;
  bool    manual_req = false;  // yeu cau bat bom o che do manual
  bool    locked     = false;  // khoa sau PUMP_TIMEOUT, can "reset"
  uint8_t active     = 0;      // canh bao dang ton tai
};

void pump_begin();
void pump_set(bool on);
bool pump_is_on();
uint32_t pump_run_ms();

// Quyet dinh thuan (khong cham phan cung), de test:
// tra ve true neu bom nen bat. 'raised' nhan cac canh bao hien tai.
bool control_decide(const Control &c, const SensorData &s, bool pump_on,
                    uint32_t run_ms, uint8_t *raised, bool *lock_now);

// Chay 1 buoc: quyet dinh + dieu khien bom. Tra ve cac canh bao MOI xuat hien.
uint8_t control_update(Control &c, const SensorData &s);

const char *alert_name(uint8_t bit);
