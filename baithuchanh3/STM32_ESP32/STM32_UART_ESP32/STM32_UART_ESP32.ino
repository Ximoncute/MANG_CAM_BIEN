// ESP32-C3 SuperMini
// RX → nhận từ STM32 TX
// TX → gửi về STM32 RX (chưa dùng)

#define RX_PIN 20  // kiểm tra chân thực tế
#define TX_PIN 21

void setup() {
  Serial.begin(115200);                 // debug ra PC
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // UART tới STM32
}

void loop() {
  if (Serial1.available()) {
    String line = Serial1.readStringUntil('\n');
    Serial.print("From STM32: ");
    Serial.println(line);
  }
}
