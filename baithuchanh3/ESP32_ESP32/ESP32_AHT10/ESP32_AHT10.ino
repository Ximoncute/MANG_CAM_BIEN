#include <Wire.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9); // SDA=8, SCL=9

  if (!aht.begin()) {
    Serial.println("Không tìm thấy AHT10!");
    while (1) delay(10);
  }
  Serial.println("AHT10 đã sẵn sàng");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  Serial.print("AHT10 T=");
  Serial.print(temp.temperature);
  Serial.print("C H=");
  Serial.print(humidity.relative_humidity);
  Serial.println("%");

  delay(20);
}
