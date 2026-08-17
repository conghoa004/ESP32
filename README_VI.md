# Hướng Dẫn Lập Trình & Mô Phỏng ESP32 (PlatformIO + VS Code + Wokwi)

[🇻🇳 Tiếng Việt](README_VI.md) | [🇬🇧 English](README.md)

Tài liệu và bộ mã nguồn mẫu hướng dẫn cách **thiết lập môi trường, lập trình và mô phỏng mạch ESP32** trực tiếp trên **Visual Studio Code** sử dụng **PlatformIO** kết hợp tiện ích mô phỏng **Wokwi**.

Bộ tài liệu này giúp bạn học tập, thực hành và phát triển các ứng dụng IoT / vi điều khiển ESP32 hoàn toàn trên môi trường ảo (không bắt buộc phải có phần cứng thật) cùng hệ thống các bài mẫu tham khảo đa dạng.

---

## 🎯 Mục đích của bộ mã nguồn

1. **Thực hành lập trình ESP32 trên VS Code**: Sử dụng PlatformIO chuyên nghiệp thay cho Arduino IDE truyền thống.
2. **Mô phỏng mạch nhanh chóng với Wokwi**: Kiểm tra logic code, kiểm tra nối dây ngoại vi và gỡ lỗi mà không sợ chập cháy phần cứng.
3. **Mô phỏng kết nối mạng thật (Wi-Fi & MQTT)**: Trải nghiệm truyền nhận dữ liệu IoT từ vi điều khiển ảo lên Internet/Cloud thời gian thực.
4. **Bộ ví dụ mẫu (Examples) phong phú**: Cung cấp sẵn các đoạn mã nguồn và sơ đồ mạch cho nhiều loại cảm biến và ngoại vi phổ biến.

---

## 🛠️ 1. Chuẩn bị môi trường (Cài đặt một lần)

Để bắt đầu, bạn chỉ cần máy tính đã cài đặt các phần mềm sau:

1. **[Visual Studio Code](https://code.visualstudio.com/)**.
2. **PlatformIO IDE Extension**:
   - Mở tab **Extensions** (`Ctrl + Shift + X`) trên VS Code.
   - Tìm kiếm `PlatformIO IDE` và nhấn **Install**.
3. **Wokwi Simulator Extension**:
   - Trong tab **Extensions**, tìm kiếm `Wokwi Simulator` và nhấn **Install**.
   - *(Lần đầu sử dụng Wokwi có thể yêu cầu bạn đăng nhập tài khoản Wokwi miễn phí để kích hoạt license mô phỏng).*

---

## 🚀 2. Quy trình Chạy mô phỏng trên VS Code

Mọi thư viện và cấu hình board đã được thiết lập sẵn trong [`platformio.ini`](platformio.ini). Bạn chỉ cần thực hiện 3 bước đơn giản:

### Bước 1: Mở thư mục dự án
- Mở VS Code > chọn **File > Open Folder...** > chọn thư mục `ESP32`.
- Đợi vài giây để PlatformIO nhận diện môi trường và tải các thư viện cần thiết.

### Bước 2: Biên dịch mã nguồn (Build)
- Nhấn tổ hợp phím **`Ctrl + Alt + B`** hoặc mở Terminal chạy:
  ```bash
  pio run
  ```
  *(PlatformIO sẽ biên dịch code và tạo file firmware trong thư mục `.pio/build/esp32doit-devkit-v1/firmware.bin`).*

### Bước 3: Khởi chạy mô phỏng Wokwi
- Nhấp mở file sơ đồ mạch [`diagram.json`](diagram.json) trong VS Code.
- Nhấn nút **Play (Start Simulation)** màu xanh ở góc phải trên màn hình hoặc nhấn phím `F1` > gõ `Wokwi: Start Simulator`.
- **Tương tác trực tiếp trên giao diện mô phỏng**:
  - Nhấp vào cảm biến **DHT22** để kéo thanh trượt thay đổi nhiệt độ / độ ẩm.
  - Xem kết quả hiển thị trên màn hình **OLED**.
  - Lắng nghe còi **Buzzer** và xem **Relay/LED** kích hoạt khi nhiệt độ vượt ngưỡng báo động.
  - Theo dõi log xuất ra ở khung **Serial Monitor**.

---

## 📋 3. Chương trình mẫu chính (`src/main.c++`)

Chương trình mặc định trong [`src/main.c++`](src/main.c++) minh họa một kịch bản IoT giám sát & cảnh báo toàn diện:

- **Đọc cảm biến**: Đọc nhiệt độ và độ ẩm từ **DHT22** (GPIO 25).
- **Màn hình OLED SSD1306 (I2C)**: Hiển thị trạng thái Wi-Fi, MQTT và giá trị cảm biến thời gian thực (SDA: GPIO 21, SCL: GPIO 22).
- **Logic cảnh báo**: Tự động bật còi **Buzzer** (GPIO 27, 2000Hz) và **Relay/LED** (GPIO 5) khi nhiệt độ quá nóng (`> 35°C`) hoặc quá lạnh (`< 18°C`).
- **Giao tiếp MQTT Cloud**:
  - **Publish**: Gửi gói tin JSON định kỳ 2s/lần: `{"temperature": 28.5, "humidity": 70.0}` lên topic `conghoa/esp32`.
  - **Subscribe**: Lắng nghe lệnh từ xa để bật/tắt thiết bị qua chuỗi `on` / `off` hoặc JSON `{"led":"on"}` / `{"led":"off"}`.
- **FreeRTOS Task**: Tách luồng đọc cảm biến & gửi MQTT chạy riêng trên **Core 0**, giữ cho luồng `loop()` không bị treo giật.

### Sơ đồ nối chân chương trình chính:
```text
           ESP32 DevKit V1
         +-----------------+
         |           3V3/5V| ---> Nguồn VCC (DHT22, OLED, Buzzer, Relay)
         |              GND| ---> Nguồn GND chung
(DATA)   | GPIO 25         | <--- DHT22 (Chân tín hiệu Out)
(SDA)    | GPIO 21         | <--- OLED Display (SDA)
(SCL)    | GPIO 22         | <--- OLED Display (SCL)
(Signal) | GPIO 27         | ---> Buzzer (Chân dương còi báo)
(IN)     | GPIO 5          | ---> Module Relay / Warning LED
         +-----------------+
```

---

## 📚 4. Kho bài mẫu tham khảo (`examples/`)

Trong thư mục [`examples/`](examples) chứa các bài tập mẫu độc lập cho từng loại ngoại vi. Bạn có thể dùng để học cách lập trình riêng lẻ từng linh kiện:

| Thư mục bài mẫu | Ngoại vi / Tính năng | Mô tả chi tiết |
| :--- | :--- | :--- |
| [`examples/buzzer/`](examples/buzzer) | **Còi Buzzer** | Phát âm thanh ngắt quãng, tạo giai điệu qua GPIO |
| [`examples/dht/`](examples/dht) | **Cảm biến DHT22** | Đọc nhiệt độ, độ ẩm và in ra Serial Monitor |
| [`examples/lcd/`](examples/lcd) | **LCD I2C 20x4** | Hiển thị chuỗi ký tự, số liệu lên màn hình LCD qua giao tiếp I2C |
| [`examples/oled/`](examples/oled) | **OLED SSD1306** | Hiển thị văn bản, căn lề và vẽ đồ họa cơ bản |
| [`examples/relay/`](examples/relay) | **Module Relay** | Điều khiển đóng ngắt relay với nút bấm/chế độ tự động |
| [`examples/rtc/`](examples/rtc) | **RTC DS1307** | Đọc ngày, tháng, năm, giờ, phút, giây từ IC thời gian thực |
| [`examples/servo/`](examples/servo) | **Động cơ Servo** | Điều khiển góc quay Servo từ $0^\circ$ đến $180^\circ$ |
| [`examples/mqtt/`](examples/mqtt) | **MQTT + LCD** | Kết nối broker MQTT, hiển thị dữ liệu nhận được lên LCD |
| [`examples/mqtt+oled/`](examples/mqtt+oled) | **MQTT + OLED** | Kết nối MQTT, hiển thị thông số và điều khiển LED |
| [`examples/web/`](examples/web) | **Web Dashboard** | Giao diện HTML/JS kết nối MQTT qua WebSocket để điều khiển |

### 💡 Cách chạy thử một bài mẫu bất kỳ:
1. Mở file mã nguồn trong thư mục ví dụ bạn muốn thử (ví dụ: `examples/servo/main.cpp` hoặc `examples/lcd/main.cpp`).
2. Sao chép (Copy) toàn bộ nội dung trong file đó và dán (Paste) đè vào file [`src/main.c++`](src/main.c++).
3. Nhấn **`Ctrl + Alt + B`** để biên dịch lại.
4. Mở file `diagram.json` tương ứng (nếu có trong thư mục ví dụ đó) hoặc chỉnh sửa linh kiện trong [`diagram.json`](diagram.json) gốc để chạy mô phỏng.

---

## 🌐 5. Thử nghiệm kết nối MQTT từ bên ngoài

Ngay cả khi chạy mô phỏng trên Wokwi, ESP32 ảo vẫn kết nối Internet thật qua mạng ảo `Wokwi-GUEST` và gửi/nhận dữ liệu với Broker MQTT thật!

Bạn có thể dùng phần mềm **[MQTTX](https://mqttx.app/)** hoặc web client công cộng để kiểm tra:

1. **Cấu hình kết nối trên MQTTX / MQTT Client**:
   - Host / Broker: `44.232.241.40` (hoặc `broker.emqx.io`, `broker.hivemq.com` tùy cấu hình trong code)
   - Port: `1883`
2. **Nhận dữ liệu từ ESP32 ảo**:
   - Subscribe topic: `conghoa/esp32`
   - Bạn sẽ thấy gói tin JSON `{ "temperature": ..., "humidity": ... }` đổ về mỗi 2 giây.
3. **Gửi lệnh điều khiển ngược lại ESP32 ảo**:
   - Publish vào topic `conghoa/esp32` với nội dung `{"led":"on"}` hoặc `{"led":"off"}`.
   - Quan sát trên Wokwi: Relay và đèn LED cảnh báo sẽ lập tức phản hồi bật/tắt theo lệnh.

---

## 🔌 6. Nạp vào Kit ESP32 thật (Tùy chọn)

Nếu bạn có sẵn phần cứng thật và muốn nạp code vào board:

1. Mở [`src/main.c++`](src/main.c++), sửa cấu hình Wi-Fi từ `Wokwi-GUEST` sang tên Wi-Fi và mật khẩu thực tế:
   ```cpp
   WiFi.begin("TEN_WIFI_NHA_BAN", "MAT_KHAU_WIFI");
   ```
2. Cắm cáp USB kết nối ESP32 với máy tính.
3. Nhấn nút **Upload** (biểu tượng mũi tên `→` ở thanh dưới VS Code) hoặc chạy lệnh:
   ```bash
   pio run --target upload
   ```
4. Mở Serial Monitor với tốc độ 115200:
   ```bash
   pio device monitor --baud 115200
   ```

---

## ❓ 7. Các lưu ý & Mẹo khắc phục sự cố

- **Wokwi báo lỗi không tìm thấy firmware.bin**:
  Hãy đảm bảo bạn đã nhấn `Ctrl + Alt + B` (hoặc `pio run`) để biên dịch thành công ít nhất một lần trước khi bấm Start Simulator.
- **Mô phỏng Wi-Fi trên Wokwi không kết nối**:
  Trong môi trường mô phỏng Wokwi, luôn luôn dùng SSID là `"Wokwi-GUEST"` và mật khẩu để trống `""`.
- **Serial Monitor in ký tự lạ / rác**:
  Đảm bảo tốc độ baud được đặt là `115200` (đã cấu hình sẵn trong `platformio.ini` qua dòng `monitor_speed = 115200`).
- **Thêm thư viện mới**:
  Chỉ cần mở file [`platformio.ini`](platformio.ini) và thêm tên thư viện vào mục `lib_deps`. PlatformIO sẽ tự động tải về khi build.
