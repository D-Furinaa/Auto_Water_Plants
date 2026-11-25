// Khai báo chân kết nối ESP32-S3 với L298N
const int ENA_PIN = 4;  // Chân PWM chỉnh tốc độ bơm
const int IN1_PIN = 5;
const int IN2_PIN = 6;

// Hàm bật bơm với tốc độ từ 0 đến 255 (255 = mạnh nhất)
void batBom(int tocDo) {
  tocDo = constrain(tocDo, 0, 255);
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, tocDo);
}

// Hàm tắt bơm
void tatBom() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, 0);
}

void setup() {
  Serial.begin(115200);

  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  // Đảm bảo bơm tắt khi mới khởi động
  tatBom();
  Serial.println("He thong dieu khien bom 12V san sang!");
}

void loop() {
  // 1. Chạy bơm tốc độ trung bình (~60%) trong 3 giây


  // 2. Chạy bơm tối đa (100%) trong 3 giây
  Serial.println("Bat bom: Toc do 100%");
  batBom(255);
}