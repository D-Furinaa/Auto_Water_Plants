# Ghi chu trong thu muc `scr`

## Gia lap cam bien dat

Gia tri mac dinh `SOIL_SIMULATION 1` trong `config.h` tao do am dat tu 20% len
70% roi giam ve 20% moi 120 giay. Firmware van ap dung nguong dieu khien may bom
binh thuong. Nguong mac dinh la bat bom duoi 30% va tat bom khi dat 60%.

Co the nhap gia tri thu cong qua Serial Monitor (baud 115200, gui dong moi):

- `soil 25` — giu do am dat gia lap o 25%.
- `soil 65` — thu nguong tat bom mac dinh.
- `soil auto` — quay lai gia lap tu dong; neu `SOIL_SIMULATION` la `0` thi doc
  cam bien dat that.
- `help` — hien lenh.

Lenh `soil` cap nhat dieu khien bom ngay khi dong lenh duoc nhan. Serial Monitor
in thoi gian xu ly (micro giay va mili giay), gia tri dat, trang thai bom truoc/
sau va che do dang chay. Chu ky cap nhat cam bien thuong la 250 ms.

**Can than:** che do nay co the bat L298N va may bom that. Ngat nguon bom hoac
thao tai khoi L298N khi thu neu khong muon bom chay. De doc cam bien dat that,
doi `SOIL_SIMULATION` thanh `0` va nap lai firmware.

## Nhiet do DHT22

Hien tai firmware khong doc nhiet do tu DHT22. `sensor_read()` chi doc do am
khong khi; `SensorData.temp` duoc giu o gia tri `NAN`, nen MQTT van gui truong
`"temp": null` de khong lam thay doi cau truc telemetry hien tai. OLED hien
`Nhiet: de sau`.

Khi muon su dung nhiet do sau nay:

1. Trong `sensor.cpp`, doc `dht.readTemperature()` va cap nhat `out.temp`.
2. Kiem tra gia tri nhiet do hop le. Neu `dht_ok` duoc dung chung cho do am va
   nhiet do, can thong nhat cach xu ly khi mot trong hai gia tri loi.
3. Trong `display.cpp`, thay dong `Nhiet: de sau` bang hien thi `s.temp`.
4. Kiem tra truong `temp` trong telemetry o `data_sender.cpp` va bien `temp`
   tren dashboard/MQTT consumer.
