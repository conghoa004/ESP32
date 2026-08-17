#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include <ArduinoJson.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// ================= DHT22 =================
#define DHTPIN 25
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= MQTT =================
const char *MQTTServer = "44.232.241.40"; // broker.hivemq.com // 44.232.241.40 // 35.172.255.22 // broker.emqx.io
const char *MQTT_Topic = "conghoa/esp32";
const char *MQTT_ID = "eb7ec38e-6d8f-439e-baff-caaf0881fb2634";

int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ================= WARNING DEVICE =================
const int buzzerPin = 27;
const int warningLedPin = 5;

// Ngưỡng nhiệt độ
const float TEMP_HIGH = 35.0;
const float TEMP_LOW = 18.0;

// ================= OLED UPDATE =================
void updateOLED(String wifiStatus, String mqttStatus, float h, float t)
{
  oled.clearDisplay();

  // WiFi
  oled.setCursor(0, 0);
  oled.print("WiFi : ");
  oled.println(wifiStatus);

  // MQTT
  oled.setCursor(0, 15);
  oled.print("MQTT : ");
  oled.println(mqttStatus);

  // Temperature
  oled.setCursor(0, 30);
  oled.print("Temp : ");
  oled.print(t, 1);
  oled.println(" C");

  // Humidity
  oled.setCursor(0, 45);
  oled.print("Humi : ");
  oled.print(h, 1);
  oled.println(" %");

  oled.display();
}

// ================= TEMPERATURE WARNING =================
void checkTemperature(float temperature)
{
  if (temperature > TEMP_HIGH)
  {
    digitalWrite(warningLedPin, HIGH);

    tone(buzzerPin, 2000);  // 2000 Hz

    Serial.println("WARNING: Temperature is TOO HIGH!");
  }
  else if (temperature < TEMP_LOW)
  {
    digitalWrite(warningLedPin, HIGH);

    tone(buzzerPin, 2000);  // 2000 Hz

    Serial.println("WARNING: Temperature is TOO LOW!");
  }
  else
  {
    digitalWrite(warningLedPin, LOW);

    noTone(buzzerPin);

    Serial.println("Temperature is NORMAL.");
  }
}

// ================= WIFI =================
void WIFIConnect()
{
  Serial.println("Connecting WiFi...");

  oled.clearDisplay();
  oled.setCursor(0, 20);
  oled.println("Connecting WiFi...");
  oled.display();

  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  updateOLED("Connected", "Disconnected", 0, 0);
}

// ================= MQTT RECONNECT =================
void MQTT_Reconnect()
{
  while (!client.connected())
  {
    Serial.println("Connecting MQTT...");

    if (client.connect(MQTT_ID))
    {
      Serial.println("MQTT connected");

      // Subscribe topic
      client.subscribe(MQTT_Topic);

      Serial.print("Subscribed: ");
      Serial.println(MQTT_Topic);

      updateOLED("Connected", "Connected", 0, 0);
    }
    else
    {
      Serial.print("MQTT failed rc=");
      Serial.println(client.state());

      updateOLED("Connected", "Retrying...", 0, 0);

      delay(2000);
    }
  }
}

// ================= MQTT PUBLISH =================
void mqttPublish(String topic, String message)
{
  if (client.connected())
  {
    client.publish(topic.c_str(), message.c_str());

    Serial.println("Published: " + message);
  }
}

// ================= MQTT CALLBACK =================
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  String stMessage;

  for (int i = 0; i < length; i++)
  {
    stMessage += (char)message[i];
  }

  Serial.println(stMessage);

  // Parse JSON
  StaticJsonDocument<256> doc;

  DeserializationError error = deserializeJson(doc, stMessage);

  if (!error)
  {
    const char *ledValue = doc["led"];

    if (ledValue != nullptr)
    {
      if (strcmp(ledValue, "on") == 0)
      {
        digitalWrite(warningLedPin, HIGH);

        Serial.println("LED warning ON");
      }
      else if (strcmp(ledValue, "off") == 0)
      {
        digitalWrite(warningLedPin, LOW);

        Serial.println("LED warning OFF");
      }
    }
  }
  else
  {
    // Cho phép nhận trực tiếp "on" / "off"
    if (stMessage == "on")
    {
      digitalWrite(warningLedPin, HIGH);

      Serial.println("LED warning ON");
    }
    else if (stMessage == "off")
    {
      digitalWrite(warningLedPin, LOW);

      Serial.println("LED warning OFF");
    }
  }
}

// ================= SEND DATA TASK =================
void SendData(void *parameter)
{
  for (;;)
  {
    // ================= READ DHT22 =================

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Kiểm tra dữ liệu DHT
    if (isnan(h) || isnan(t))
    {
      Serial.println("Failed to read from DHT22!");

      // Tắt cảnh báo khi cảm biến lỗi
      digitalWrite(buzzerPin, LOW);
      digitalWrite(warningLedPin, LOW);

      vTaskDelay(5000 / portTICK_PERIOD_MS);

      continue;
    }

    // ================= SERIAL =================

    Serial.println("==============================");

    Serial.print("Temperature: ");
    Serial.print(t, 1);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(h, 1);
    Serial.println(" %");

    // ================= TEMPERATURE WARNING =================

    checkTemperature(t);

    // ================= OLED =================

    String wifiStatus =
        WiFi.status() == WL_CONNECTED
            ? "Connected"
            : "Disconnected";

    String mqttStatus =
        client.connected()
            ? "Connected"
            : "Disconnected";

    updateOLED(
        wifiStatus,
        mqttStatus,
        h,
        t);

    // ================= JSON =================

    StaticJsonDocument<256> doc;

    doc["temperature"] = t;
    doc["humidity"] = h;

    String message;

    serializeJson(doc, message);

    // ================= MQTT =================

    mqttPublish(MQTT_Topic, message);

    Serial.println("MQTT Data:");
    Serial.println(message);

    Serial.println("==============================");

    // ================= DELAY =================

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);

  // ================= OLED INIT =================

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED fail");

    while (1)
    {
      delay(1000);
    }
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println("Starting...");
  oled.display();

  // ================= DHT INIT =================

  dht.begin();

  // ================= GPIO INIT =================

  pinMode(buzzerPin, OUTPUT);
  pinMode(warningLedPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);
  digitalWrite(warningLedPin, LOW);

  // ================= WIFI =================

  WIFIConnect();

  // ================= MQTT =================

  client.setServer(MQTTServer, Port);
  client.setCallback(callback);

  // ================= FREERTOS TASK =================

  xTaskCreatePinnedToCore(
      SendData,
      "SendData",
      10000,
      NULL,
      1,
      NULL,
      0);
}

// ================= LOOP =================
void loop()
{
  // Kiểm tra WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    WIFIConnect();
  }

  // Kiểm tra MQTT
  if (!client.connected())
  {
    MQTT_Reconnect();
  }

  // Xử lý MQTT
  client.loop();
}