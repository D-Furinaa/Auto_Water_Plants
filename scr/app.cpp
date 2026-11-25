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
static char       g_serial_line[48];
static size_t     g_serial_line_length = 0;

static void update_control_from_serial() {
  const uint32_t started_us = micros();
  const bool pump_before = pump_is_on();
  sensor_read(g_sensor);
  g_pending_alerts |= control_update(g_ctl, g_sensor);
  const uint32_t elapsed_us = micros() - started_us;
  display_update(g_sensor, g_ctl, pump_is_on(), net_connected());
  Serial.printf("[serial] xu ly dieu khien: %lu us (%.3f ms), dat=%.1f%%, bom=%s%s, mode=%s\n",
                (unsigned long)elapsed_us, elapsed_us / 1000.0f, g_sensor.soil_pct,
                pump_is_on() ? "ON" : "OFF",
                pump_before != pump_is_on() ? " (da doi trang thai)" : "",
                g_ctl.mode == MODE_AUTO ? "AUTO" : "MANUAL");
}

void app_setup() {
  Serial.begin(115200);
  g_ctl.low  = SOIL_LOW_DEFAULT;
  g_ctl.high = SOIL_HIGH_DEFAULT;
  pump_begin();           // bom tat truoc tien
  sensor_begin();
  display_begin();
  cmd_begin(&g_ctl);
  net_begin();
  Serial.println("[serial] Lenh: soil <0-100>, soil auto, help");
}

static void handle_serial_line(char *line) {
  char *command = strtok(line, " \t");
  if (!command) return;

  if (strcmp(command, "help") == 0 && !strtok(nullptr, " \t")) {
    Serial.println("[serial] soil <0-100> de dat do am gia lap; soil auto de quay lai che do mac dinh");
    return;
  }

  char *value = strtok(nullptr, " \t");
  char *extra = strtok(nullptr, " \t");
  if (strcmp(command, "soil") != 0 || !value || extra) {
    Serial.println("[serial] Lenh khong hop le. Nhap: soil <0-100>, soil auto, help");
    return;
  }

  if (strcmp(value, "auto") == 0) {
    sensor_clear_soil_override();
    Serial.println("[serial] Da bat lai gia lap tu dong/cam bien that.");
    update_control_from_serial();
    return;
  }

  char *end = nullptr;
  float soil_pct = strtof(value, &end);
  if (*end != '\0' || !sensor_set_soil_override(soil_pct)) {
    Serial.println("[serial] Do am phai la so tu 0 den 100.");
    return;
  }

  Serial.printf("[serial] Da dat do am dat gia lap: %.1f%%\n", soil_pct);
  update_control_from_serial();
}

static void poll_serial_commands() {
  while (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == '\r') continue;
    if (ch == '\n') {
      g_serial_line[g_serial_line_length] = '\0';
      handle_serial_line(g_serial_line);
      g_serial_line_length = 0;
    } else if (g_serial_line_length < sizeof(g_serial_line) - 1) {
      g_serial_line[g_serial_line_length++] = ch;
    } else {
      g_serial_line_length = 0;
      Serial.println("[serial] Lenh qua dai, vui long thu lai.");
    }
  }
}

static void flush_alerts() {
  if (!g_pending_alerts || !net_connected()) return;
  for (uint8_t bit = 1; bit; bit <<= 1) {
    if ((g_pending_alerts & bit) && publish_alert(alert_name(bit))) g_pending_alerts &= ~bit;
  }
}

void app_loop() {
  poll_serial_commands();
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
