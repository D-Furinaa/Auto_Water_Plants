#pragma once
// ---- Cau hinh toan he thong (ESP32-S3, 1 chau cay) ----

// --- Chan GPIO (theo README) ---
#define PIN_LED_GREEN 39  // OK
#define PIN_LED_YELLOW 40 // dang tuoi
#define PIN_LED_RED 41    // canh bao / loi
#define PIN_SOIL 1        // do am dat (ADC)
#define PIN_DHT 2         // DHT22
#define PIN_OLED_SCL 4
#define PIN_OLED_SDA 5
#define PIN_L298N_EN 7  // ENA: bat/tat kenh A (thao jumper ENA tren module)
#define PIN_L298N_IN1 8 // IN1: chieu quay bom
#define PIN_L298N_IN2 9 // IN2: chieu quay bom
#define PIN_WATER 6 // cam bien muc nuoc (so): LOW = het nuoc

// --- Gia lap cam bien dat de thu he thong ---
#define SOIL_SIMULATION 1 // doi thanh 0 de doc cam bien dat that
#define SOIL_SIMULATION_MIN_PCT 20.0f
#define SOIL_SIMULATION_MAX_PCT 70.0f
#define SOIL_SIMULATION_PERIOD_MS 120000UL // chu ky 20% -> 70% -> 20%

// --- Hieu chuan do am dat (ADC 12-bit). PHAI do lai tren cam bien thuc te ---
#define SOIL_ADC_DRY 3200 // gia tri ADC khi de ngoai khong khi (0%)
#define SOIL_ADC_WET 1400 // gia tri ADC khi ngam nuoc (100%)

// --- Nguong tuoi mac dinh (%), co the doi qua MQTT ---
#define SOIL_LOW_DEFAULT 30.0f  // duoi nguong -> bat bom
#define SOIL_HIGH_DEFAULT 60.0f // dat nguong -> tat bom

// --- An toan ---
#define PUMP_MAX_RUN_MS 60000UL // bom chay lien tuc toi da

// --- Chu ky ---
#define SENSOR_PERIOD_MS 250UL
#define TELEMETRY_PERIOD_MS 10000UL

// --- WiFi / MQTT (sua truoc khi nap) ---
#define WIFI_SSID "huy"
#define WIFI_PASS "23052005"
#define MQTT_HOST "172.172.12.24"
#define MQTT_PORT 1883
#define MQTT_USER "" // de trong neu broker khong can xac thuc
#define MQTT_PASS ""
#define PLANT_ID "1" // doi so chau khi mo rong
#define TOPIC_PREFIX "plant/" PLANT_ID "/"
