#include "DHT.h"

// Khai báo chân kết nối DHT22 và loại cảm biến DHT22
#define DHTPIN 19 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  
  // Khởi tạo cảm biến DHT22
  dht.begin();
}

void loop() {
  // Đọc độ ẩm
  float h = dht.readHumidity();
  // Đọc nhiệt độ dưới dạng Celsius (isFahrenheit = false)
  float t = dht.readTemperature();
  // Đọc nhiệt độ dưới dạng Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Kiểm tra nếu có lỗi khi đọc dữ liệu từ cảm biến
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Tính toán chỉ số nhiệt (heat index) dựa trên nhiệt độ Fahrenheit và độ ẩm
  float hif = dht.computeHeatIndex(f, h);
  // Tính toán chỉ số nhiệt (heat index) dựa trên nhiệt độ Celsius và độ ẩm
  float hic = dht.computeHeatIndex(t, h, false);

  // In kết quả ra Serial Monitor
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  delay(2000); // Đợi 2 giây trước khi đọc lại dữ liệu từ cảm biến
}