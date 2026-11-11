#pragma once
#include <Arduino.h>
#include "sensor.h"
#include "motor_c.h"

void display_begin();
// alerts = c.active; net_ok = WiFi+MQTT da ket noi
void display_update(const SensorData &s, const Control &c, bool pump_on, bool net_ok);
