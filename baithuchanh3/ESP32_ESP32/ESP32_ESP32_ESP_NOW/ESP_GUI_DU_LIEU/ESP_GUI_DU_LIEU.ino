#include <WiFi.h>        // Thư viện WiFi cần thiết để cấu hình chế độ WiFi cho ESP32
#include <esp_now.h>     // Thư viện giao thức ESP-NOW của Espressif

// ========================
// 🔧 CẤU HÌNH THÔNG TIN NHẬN
// ========================

// 👉 Địa chỉ MAC của ESP nhận (bạn cần thay đúng MAC thật in ra từ ESP nhận)
uint8_t receiverAddress[] = {0xDC, 0x06, 0x75, 0xF6, 0x2F, 0x98};

// ========================
// 📦 ĐỊNH NGHĨA DỮ LIỆU TRUYỀN
// ========================

// Tạo một cấu trúc chứa dữ liệu cần gửi đi (nhiệt độ, độ ẩm)
typedef struct struct_message {
  float temperature;   // Biến lưu nhiệt độ (°C)
  float humidity;      // Biến lưu độ ẩm (%)
} struct_message;

// Tạo một biến toàn cục để lưu dữ liệu sẽ gửi
struct_message dataToSend;

// ========================
// 📡 CALLBACK: THÔNG BÁO KHI GỬI XONG
// ========================

// Hàm callback được gọi tự động khi ESP gửi dữ liệu xong
// Tham số:
// - info: thông tin gói tin (wifi_tx_info_t)
// - status: kết quả gửi (thành công hay thất bại)
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("📤 Trạng thái gửi: ");
  // In ra kết quả gửi
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "✅ Thành công" : "❌ Thất bại");
}

// ========================
// ⚙️ HÀM setup()
// ========================
void setup() {
  Serial.begin(115200);       // Bật Serial để xem thông báo trên Serial Monitor
  WiFi.mode(WIFI_STA);        // Đặt chế độ WiFi của ESP thành Station (STA)

  // Khởi tạo ESP-NOW
  if (esp_now_init() != ESP_OK) {  // Nếu khởi tạo thất bại
    Serial.println("❌ ESP-NOW init thất bại!");
    return;  // Dừng chương trình
  }

  // Đăng ký hàm callback khi gửi xong
  esp_now_register_send_cb(OnDataSent);

  // ========================
  // 👥 THÊM PEER (THIẾT BỊ NHẬN)
  // ========================
  esp_now_peer_info_t peerInfo = {};                // Tạo cấu trúc lưu thông tin peer
  memcpy(peerInfo.peer_addr, receiverAddress, 6);   // Sao chép địa chỉ MAC của thiết bị nhận
  peerInfo.channel = 0;                             // Dùng kênh WiFi mặc định
  peerInfo.encrypt = false;                         // Không mã hóa dữ liệu (cho test đơn giản)

  // Thêm thiết bị nhận vào danh sách peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("❌ Không thể thêm peer!");
    return;  // Dừng chương trình nếu lỗi
  }

  Serial.println("✅ ESP gửi sẵn sàng!");  // Thông báo sẵn sàng gửi dữ liệu
}

// ========================
// 🔁 HÀM loop()
// ========================
void loop() {
  // --------------------
  // 🧮 TẠO DỮ LIỆU GIẢ LẬP
  // --------------------
  dataToSend.temperature = random(200, 300) / 10.0; // Tạo ngẫu nhiên nhiệt độ từ 20.0 đến 30.0 °C
  dataToSend.humidity = random(400, 800) / 10.0;   // Tạo ngẫu nhiên độ ẩm từ 40.0 đến 80.0 %

  // --------------------
  // 🚀 GỬI DỮ LIỆU QUA ESP-NOW
  // --------------------
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));

  // --------------------
  // 🧾 KIỂM TRA KẾT QUẢ GỬI
  // --------------------
  if (result == ESP_OK) {
    // Nếu gửi thành công (chưa chắc nhận được, chỉ là gói tin gửi đi thành công)
    Serial.printf("📦 Đã gửi: %.2f °C, %.2f %%RH\n", dataToSend.temperature, dataToSend.humidity);
  } else {
    // Nếu có lỗi trong quá trình gửi (ví dụ chưa thêm peer)
    Serial.println("⚠️ Gửi dữ liệu thất bại!");
  }

  delay(2000);  // Gửi lại sau mỗi 2 giây
}
