#include "app.h"
#include "config.h"
#include "sensor.h"
#include "motor_c.h"
#include "display.h"
#include "data_sender.h"
#include "data_received.h"

static Control    g_ctl;
static SensorData g_sensor;
static uint8_t    g_pending_alerts = 0;   // canh bao chua gui duoc (mat mang)
static uint32_t   g_t_sensor = 0, g_t_telemetry = 0;

void app_setup() {
  Serial.begin(115200);
  g_ctl.low  = SOIL_LOW_DEFAULT;
  g_ctl.high = SOIL_HIGH_DEFAULT;
  pump_begin();           // bom tat truoc tien
  sensor_begin();
  display_begin();
  cmd_begin(&g_ctl);
  net_begin();
}

static void flush_alerts() {
  if (!g_pending_alerts || !net_connected()) return;
  for (uint8_t bit = 1; bit; bit <<= 1) {
    if ((g_pending_alerts & bit) && publish_alert(alert_name(bit))) g_pending_alerts &= ~bit;
  }
}

void app_loop() {
  net_loop();
  uint32_t now = millis();

  if (now - g_t_sensor >= SENSOR_PERIOD_MS) {
    g_t_sensor = now;
    sensor_read(g_sensor);
    g_pending_alerts |= control_update(g_ctl, g_sensor);
    display_update(g_sensor, g_ctl, pump_is_on(), net_connected());
  } else if (pump_is_on()) {
    // Giua 2 lan doc: van kiem tra timeout bom de phan ung nhanh
    g_pending_alerts |= control_update(g_ctl, g_sensor);
  }

  flush_alerts();

  if (now - g_t_telemetry >= TELEMETRY_PERIOD_MS) {
    g_t_telemetry = now;
    publish_telemetry(g_sensor, g_ctl, pump_is_on());
  }
}
