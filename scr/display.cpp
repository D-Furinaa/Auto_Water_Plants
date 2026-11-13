#include "display.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 oled(128, 64, &Wire, -1);
static bool oled_ok = false;

void display_begin() {
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  oled_ok = oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (oled_ok) { oled.clearDisplay(); oled.display(); }
}

void display_update(const SensorData &s, const Control &c, bool pump_on, bool net_ok) {
  digitalWrite(PIN_LED_GREEN,  c.active == 0 ? HIGH : LOW);
  digitalWrite(PIN_LED_YELLOW, pump_on ? HIGH : LOW);
  digitalWrite(PIN_LED_RED,    c.active != 0 ? HIGH : LOW);

  if (!oled_ok) return;
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);

  oled.setCursor(0, 0);
  oled.print(c.mode == MODE_AUTO ? "AUTO" : "MANUAL");
  oled.setCursor(70, 0);
  oled.print(net_ok ? "MQTT:OK" : "MQTT:--");

  oled.setCursor(0, 14);
  oled.print("Dat : ");
  if (s.soil_ok) oled.printf("%.0f%%", s.soil_pct); else oled.print("LOI");

  oled.setCursor(0, 26);
  oled.print("Nhiet: ");
  if (s.dht_ok) oled.printf("%.1fC", s.temp); else oled.print("LOI");

  oled.setCursor(0, 38);
  oled.print("KK  : ");
  if (s.dht_ok) oled.printf("%.0f%%", s.hum); else oled.print("LOI");

  oled.setCursor(0, 50);
  oled.printf("Bom:%s Nuoc:%s", pump_on ? "ON" : "OFF", s.water_ok ? "OK" : "HET");
  oled.display();
}
