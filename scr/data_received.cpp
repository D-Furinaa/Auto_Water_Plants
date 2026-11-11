#include "data_received.h"
#include "config.h"
#include <ArduinoJson.h>

static Control *g_ctl = nullptr;

void cmd_begin(Control *ctl) { g_ctl = ctl; }

void cmd_subscribe(PubSubClient &mqtt) {
  mqtt.subscribe(TOPIC_PREFIX "cmd/#", 1);
}

bool cmd_apply(Control &c, const String &name, const String &payload) {
  if (name == "mode") {
    if (payload == "auto")        c.mode = MODE_AUTO;
    else if (payload == "manual") c.mode = MODE_MANUAL;
    else return false;
    c.manual_req = false;          // doi che do luon tat bom thu cong
    return true;
  }
  if (name == "pump") {
    if (payload == "reset") { c.locked = false; return true; }
    if (c.mode != MODE_MANUAL) return false;   // chi nhan o che do manual
    if (payload == "on")       c.manual_req = true;
    else if (payload == "off") c.manual_req = false;
    else return false;
    return true;
  }
  if (name == "threshold") {
    JsonDocument doc;
    if (deserializeJson(doc, payload)) return false;
    float lo = doc["low"] | c.low;
    float hi = doc["high"] | c.high;
    if (lo < 0 || hi > 100 || lo >= hi) return false;
    c.low = lo; c.high = hi;
    return true;
  }
  return false;
}

void cmd_on_message(char *topic, uint8_t *payload, unsigned int len) {
  if (!g_ctl) return;
  String t(topic);
  String name = t.substring(strlen(TOPIC_PREFIX "cmd/"));
  String body;
  body.reserve(len);
  for (unsigned int i = 0; i < len; i++) body += (char)payload[i];
  body.trim();
  bool ok = cmd_apply(*g_ctl, name, body);
  Serial.printf("[cmd] %s=%s -> %s\n", name.c_str(), body.c_str(), ok ? "ok" : "rejected");
}
