#pragma once
#include <Arduino.h>
#include "sensor.h"
#include "motor_c.h"

void net_begin();
void net_loop();            // khong chan; tu reconnect WiFi/MQTT
bool net_connected();
bool publish_telemetry(const SensorData &s, const Control &c, bool pump_on);
bool publish_alert(const char *type);
