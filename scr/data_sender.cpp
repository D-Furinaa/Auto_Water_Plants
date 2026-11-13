#include "data_sender.h"
#include "data_received.h"
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>

static WiFiClient   g_wifi;
static PubSubClient g_mqtt(g_wifi);
static uint32_t     g_last_try = 0;

void net_begin() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  g_mqtt.setServer(MQTT_HOST, MQTT_PORT);
  g_mqtt.setBufferSize(512);
  g_mqtt.setCallback(cmd_on_message);
}

bool net_connected() { return WiFi.status() == WL_CONNECTED && g_mqtt.connected(); }

static void mqtt_try_connect() {
  String cid = "plant-" PLANT_ID "-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  bool ok;
  const char *lwt = TOPIC_PREFIX "status";
  if (strlen(MQTT_USER)) ok = g_mqtt.connect(cid.c_str(), MQTT_USER, MQTT_PASS, lwt, 1, true, "offline");
  else                   ok = g_mqtt.connect(cid.c_str(), nullptr, nullptr, lwt, 1, true, "offline");
  if (ok) {
    g_mqtt.publish(lwt, "online", true);
    cmd_subscribe(g_mqtt);
    Serial.println("[mqtt] connected");
  } else {
    Serial.printf("[mqtt] fail rc=%d\n", g_mqtt.state());
  }
}

void net_loop() {
  if (WiFi.status() != WL_CONNECTED) return;     // setAutoReconnect lo phan WiFi
  if (!g_mqtt.connected()) {
    if (millis() - g_last_try >= 5000) { g_last_try = millis(); mqtt_try_connect(); }
    return;
  }
  g_mqtt.loop();
}

static void put_num(char *buf, size_t n, const char *key, float v, int dec) {
  size_t len = strlen(buf);
  if (isnan(v)) snprintf(buf + len, n - len, "\"%s\":null,", key);
  else          snprintf(buf + len, n - len, "\"%s\":%.*f,", key, dec, v);
}

bool publish_telemetry(const SensorData &s, const Control &c, bool pump_on) {
  if (!g_mqtt.connected()) return false;
  char buf[320] = "{";
  put_num(buf, sizeof buf, "soil", s.soil_pct, 1);
  put_num(buf, sizeof buf, "temp", s.temp, 1);
  put_num(buf, sizeof buf, "hum", s.hum, 1);
  size_t len = strlen(buf);
  snprintf(buf + len, sizeof buf - len,
           "\"water_ok\":%s,\"pump\":%s,\"mode\":\"%s\",\"low\":%.0f,\"high\":%.0f,\"alerts\":%u}",
           s.water_ok ? "true" : "false", pump_on ? "true" : "false",
           c.mode == MODE_AUTO ? "auto" : "manual", c.low, c.high, c.active);
  return g_mqtt.publish(TOPIC_PREFIX "telemetry", buf);
}

bool publish_alert(const char *type) {
  if (!g_mqtt.connected()) return false;
  char buf[64];
  snprintf(buf, sizeof buf, "{\"type\":\"%s\"}", type);
  return g_mqtt.publish(TOPIC_PREFIX "alert", buf);
}
