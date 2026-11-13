#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include "motor_c.h"

// Gan doi tuong Control de lenh MQTT chinh sua.
void cmd_begin(Control *ctl);
// Goi sau moi lan ket noi MQTT thanh cong.
void cmd_subscribe(PubSubClient &mqtt);
// Callback cho PubSubClient.
void cmd_on_message(char *topic, uint8_t *payload, unsigned int len);

// Ham thuan de test: xu ly lenh theo ten (vd "mode", "pump", "threshold").
// Tra ve true neu lenh hop le va da ap dung.
bool cmd_apply(Control &c, const String &name, const String &payload);
