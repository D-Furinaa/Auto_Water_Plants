#include "motor_c.h"
#include "config.h"

static bool     g_on = false;
static uint32_t g_start = 0;

void pump_begin() {
  pinMode(PIN_PUMP, OUTPUT);
  digitalWrite(PIN_PUMP, !PUMP_ACTIVE_LEVEL);
  g_on = false;
}

void pump_set(bool on) {
  if (on == g_on) return;
  g_on = on;
  if (on) g_start = millis();
  digitalWrite(PIN_PUMP, on ? PUMP_ACTIVE_LEVEL : !PUMP_ACTIVE_LEVEL);
}

bool pump_is_on() { return g_on; }
uint32_t pump_run_ms() { return g_on ? millis() - g_start : 0; }

bool control_decide(const Control &c, const SensorData &s, bool pump_on,
                    uint32_t run_ms, uint8_t *raised, bool *lock_now) {
  uint8_t r = 0;
  bool want = pump_on;
  *lock_now = false;

  if (!s.soil_ok) r |= ALERT_SENSOR_FAIL;

  if (c.mode == MODE_AUTO) {
    if (!s.soil_ok)              want = false;           // khong tuoi mu
    else if (s.soil_pct < c.low) want = true;
    else if (s.soil_pct >= c.high) want = false;         // giua 2 nguong: giu nguyen
  } else {
    want = c.manual_req;
  }

  if (!s.water_ok) { r |= ALERT_NO_WATER; want = false; }

  if (pump_on && run_ms >= PUMP_MAX_RUN_MS) { *lock_now = true; }
  if (c.locked || *lock_now) { r |= ALERT_PUMP_TIMEOUT; want = false; }

  *raised = r;
  return want;
}

uint8_t control_update(Control &c, const SensorData &s) {
  uint8_t raised; bool lock_now;
  bool want = control_decide(c, s, pump_is_on(), pump_run_ms(), &raised, &lock_now);
  if (lock_now) { c.locked = true; c.manual_req = false; }
  pump_set(want);
  uint8_t fresh = raised & ~c.active;
  c.active = raised;
  return fresh;
}

const char *alert_name(uint8_t bit) {
  switch (bit) {
    case ALERT_SENSOR_FAIL:  return "SENSOR_FAIL";
    case ALERT_NO_WATER:     return "NO_WATER";
    case ALERT_PUMP_TIMEOUT: return "PUMP_TIMEOUT";
    default:                 return "UNKNOWN";
  }
}
