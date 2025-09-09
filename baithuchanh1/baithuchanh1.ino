#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Chọn chân DATA cho DHT11
#define DHTPIN 4        
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(2000); // chờ ổn định serial
  Serial.println("===== Bắt đầu đọc cảm biến DHT11 =====");
  dht.begin();
}

void loop() {
  Serial.println("Đang đọc dữ liệu...");
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("❌ Lỗi: Không đọc được từ DHT11 (kiểm tra dây nối & GPIO)!");
  } else {
    Serial.print("🌡 Nhiệt độ: ");
    Serial.print(t);
    Serial.print(" °C  |  💧 Độ ẩm: ");
    Serial.print(h);
    Serial.println(" %");
  }

  delay(2000); // DHT11 chỉ cho đọc mỗi 1-2 giây
}
