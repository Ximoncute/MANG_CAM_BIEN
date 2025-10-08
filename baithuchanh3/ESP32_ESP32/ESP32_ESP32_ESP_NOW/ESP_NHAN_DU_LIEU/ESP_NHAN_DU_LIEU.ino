#include <WiFi.h>         // Thư viện WiFi để điều khiển chế độ mạng (STA, AP, v.v)
#include <esp_now.h>      // Thư viện ESP-NOW dùng cho giao tiếp không cần WiFi Router

// ===============================
// ⚙️ Cấu trúc dữ liệu sẽ nhận
// ===============================
typedef struct struct_message {
  float temperature;      // Nhiệt độ (°C)
  float humidity;         // Độ ẩm (%)
} struct_message;

struct_message incomingData;  // Biến chứa dữ liệu nhận được

// ==================================================
// 📥 Hàm callback được gọi tự động khi có dữ liệu đến
// ==================================================
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int len) {
  // Sao chép dữ liệu nhận được (từ bộ nhớ tạm) vào biến incomingData
  memcpy(&incomingData, data, sizeof(incomingData));

  // In ra màn hình Serial để kiểm tra
  Serial.printf("📩 Nhận dữ liệu: %.2f °C, %.2f %%RH\n",
                incomingData.temperature, incomingData.humidity);
}

// ===============================
// 🚀 Hàm setup() chạy 1 lần duy nhất khi ESP khởi động
// ===============================
void setup() {
  Serial.begin(115200);   // Bật cổng Serial để in ra thông tin debug
  WiFi.mode(WIFI_STA);    // Đặt ESP vào chế độ Station (bắt buộc với ESP-NOW)

  // Khởi tạo giao thức ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init thất bại!");  // Thông báo nếu khởi tạo lỗi
    return;                                       // Dừng chương trình setup
  }

  // Đăng ký hàm callback khi có dữ liệu đến
  esp_now_register_recv_cb(OnDataRecv);

  // Thông báo khởi động thành công
  Serial.println("✅ ESP nhận sẵn sàng!");

  // In địa chỉ MAC của ESP này để gửi cho ESP gửi dữ liệu biết
  Serial.print("📡 MAC của ESP nhận: ");
  Serial.println(WiFi.macAddress());
}

// ===============================
// 🔁 Hàm loop() chạy liên tục sau setup()
// ===============================
void loop() {
  // ESP nhận không cần lệnh lặp đặc biệt, chỉ cần đợi callback hoạt động
  delay(2000);  // Delay nhẹ để giảm tải CPU
}
