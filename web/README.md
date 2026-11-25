# Web dashboard va database

Ung dung nay nhan MQTT telemetry tu ESP32, luu moi ban tin vao SQLite va hien thi
du lieu moi nhat/lich su tren dashboard.

## Chay ung dung

Can Node.js 22.13 tro len (SQLite dung module co san `node:sqlite`) va MQTT
broker co the truy cap tu may chay web.

```sh
cd web
npm install
cp .env.example .env
npm start
```

Mo `http://localhost:3000`. Sua `MQTT_URL`, `MQTT_USERNAME` va `MQTT_PASSWORD`
trong `.env` neu broker khac; mac dinh URL la `mqtt://172.172.12.24:1883`.
Database duoc tao tu dong tai `web/data/plant-data.sqlite`. Thu muc `data` va
file `.env` khong duoc dua len git.

## Luong du lieu

ESP32 publish JSON len topic `plant/<plantId>/telemetry`, vi du
`plant/1/telemetry`. Backend dang ky topic `plant/+/telemetry` va luu cac truong
`soil`, `temp`, `hum`, `water_ok`, `pump`, `mode`, `low`, `high`, `alerts`
cung thoi diem nhan. Cac truong khong co trong payload duoc luu la `NULL`.
Dashboard goi API moi 5 giay de doc trang thai moi nhat va 100 mau gan nhat.

API:

- `GET /api/status` — trang thai ket noi MQTT.
- `GET /api/telemetry/latest` — ban tin moi nhat; tuy chon `?plantId=1`.
- `GET /api/telemetry/history?limit=100` — lich su; `limit` tu 1 den 500 va
  co the loc bang `plantId`.

Chay kiem tra parser MQTT bang `npm test`.

## Luu y firmware hien tai

Firmware can build duoc va publish JSON telemetry len dung topic. Trong project
hien tai, `sensor.h` dang comment cac truong `temp`, `hum`, `dht_ok`, `water_ok`,
nhung `display.cpp`, `motor_c.cpp` va `data_sender.cpp` van tham chieu cac truong
do. Can dong bo cac file firmware truoc khi nap ESP32; dashboard co the nhan
payload chi co `soil`, cac truong con lai se hien thi `--`.
