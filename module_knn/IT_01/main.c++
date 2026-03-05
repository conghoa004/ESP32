#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// LCD I2C
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// MQTT
const char *MQTTServer = "44.232.241.40"; // broker.hivemq.com // 44.232.241.40 // 35.172.255.22 // broker.emqx.io
const char *MQTT_Topic = "conghoa/esp32";
const char *MQTT_ID = "eb7ec38e-6d8f-439e-baff-caaf0881fb2ghg6";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 2;
bool ledStatus = false;

// Hàm cập nhật LCD với trạng thái WiFi, MQTT
void updateLCD(String wifiStatus, String mqttStatus)
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("WiFi : " + wifiStatus);

  lcd.setCursor(0, 1);
  lcd.print("MQTT : " + mqttStatus);
}

// Kêt nối WiFi
void WIFIConnect()
{
  Serial.println("Connecting to SSID: Wokwi-GUEST");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");

  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  updateLCD("Connected", "Disconnected");
  Serial.println("");
  Serial.print("WiFi connected");
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

// Kết nối MQTT
void MQTT_Reconnect()
{
  while (!client.connected())
  {
    if (client.connect(MQTT_ID))
    {
      Serial.print("MQTT Topic: ");
      Serial.print(MQTT_Topic);
      Serial.print(" connected");

      // Đăng ký subscribe topic
      client.subscribe(MQTT_Topic);
      client.subscribe("door/access");

      updateLCD("Connected", "Connected");
      Serial.println("");
    }
    else
    {
      updateLCD("Connected", "Retrying...");
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(2000);
    }
  }
}

// Hàm gửi tin nhắn MQTT
void mqttPublish(String topic, String message)
{
  if (client.connected())
  {
    client.publish(topic.c_str(), message.c_str());
    Serial.print("Message published: ");
    Serial.println(message);
  }
  else
  {
    Serial.println("MQTT not connected. Message not published.");
  }
}

// Hàm callback khi nhận được tin nhắn MQTT
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String stMessage;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();

  // Xử lý tin nhắn nhận được chuyển thành JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, stMessage);

  if (!error)
  {
    // Trường hợp là JSON
    Serial.println("JSON received");

    // Lấy giá trị của trường "led"
    const char *ledValue = doc["led"];

    if (strcmp(ledValue, "on") == 0 && strcmp(topic, MQTT_Topic) == 0)
    {
      digitalWrite(ledPin, HIGH);
      ledStatus = true;
    }
    else if (strcmp(ledValue, "off") == 0 && strcmp(topic, MQTT_Topic) == 0)
    {
      digitalWrite(ledPin, LOW);
      ledStatus = false;
    }
  }
  else
  {
    // Trường hợp là plain text
    Serial.println("Plain text received");

    stMessage.trim();

    if (stMessage.startsWith("OPEN:"))
    {

      String name = stMessage.substring(5);

      digitalWrite(ledPin, HIGH);

      lcd.setCursor(0, 2);
      lcd.print("ACCESS");

      lcd.setCursor(0, 3);
      lcd.print(name);

      delay(3000);

      digitalWrite(ledPin, LOW);
    }

    // ================= ACCESS DENIED =================
    if (stMessage == "DENIED")
    {

      lcd.setCursor(0, 2);
      lcd.print("ACCESS DENIED");

      delay(3000);
    }
  }

  updateLCD("Connected", "Connected");
}

// Setup
void setup()
{
  // Kết nối Serial
  Serial.begin(115200);

  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();

  // Khởi tạo các thiết bị ngoại vi
  pinMode(ledPin, OUTPUT);

  // Kết nối WiFi
  WIFIConnect();

  // Cấu hình MQTT
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);
}

// Loop
void loop()
{
  // Kiểm tra kết nối WiFi, nếu mất kết nối thì thực hiện kết nối lại
  if (WiFi.status() != WL_CONNECTED)
  {
    WIFIConnect();
  }

  // Kiểm tra kết nối MQTT, nếu mất kết nối thì thực hiện kết nối lại
  if (!client.connected())
  {
    MQTT_Reconnect();
  }

  // Xử lý các sự kiện MQTT
  client.loop();
}