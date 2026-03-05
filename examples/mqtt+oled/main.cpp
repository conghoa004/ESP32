#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include <ArduinoJson.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MQTT
const char *MQTTServer = "broker.emqx.io"; // broker.hivemq.com // 44.232.241.40 // 35.172.255.22 // broker.emqx.io
const char *MQTT_Topic = "conghoa/esp32";
const char *MQTT_ID = "eb7ec38e-6d8f-439e-baff-caaf0881fb2634";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 2;
bool ledStatus = false;

// ================= OLED Update =================
void updateOLED(String wifiStatus, String mqttStatus)
{
  oled.clearDisplay();

  oled.setCursor(0, 0);
  oled.print("WiFi : ");
  oled.println(wifiStatus);

  oled.setCursor(0, 15);
  oled.print("MQTT : ");
  oled.println(mqttStatus);

  oled.display();
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

  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  updateOLED("Connected", "Disconnected");
}

// ================= MQTT =================
void MQTT_Reconnect()
{
  while (!client.connected())
  {
    Serial.println("Connecting MQTT...");

    if (client.connect(MQTT_ID))
    {
      Serial.println("MQTT connected");

      client.subscribe(MQTT_Topic);

      updateOLED("Connected", "Connected");
    }
    else
    {
      Serial.print("MQTT failed rc=");
      Serial.println(client.state());

      updateOLED("Connected", "Retrying...");
      delay(2000);
    }
  }
}

// ================= MQTT Publish =================
void mqttPublish(String topic, String message)
{
  if (client.connected())
  {
    client.publish(topic.c_str(), message.c_str());
    Serial.println("Published: " + message);
  }
}

// ================= MQTT Callback =================
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived: ");

  String stMessage;

  for (int i = 0; i < length; i++)
  {
    stMessage += (char)message[i];
  }

  Serial.println(stMessage);

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, stMessage);

  if (!error)
  {
    const char *ledValue = doc["led"];

    if (strcmp(ledValue, "on") == 0)
    {
      digitalWrite(ledPin, HIGH);
      ledStatus = true;
    }
    else if (strcmp(ledValue, "off") == 0)
    {
      digitalWrite(ledPin, LOW);
      ledStatus = false;
    }
  }
  else
  {
    if (stMessage == "on")
    {
      digitalWrite(ledPin, HIGH);
      ledStatus = true;
    }
    else if (stMessage == "off")
    {
      digitalWrite(ledPin, LOW);
      ledStatus = false;
    }
  }

  updateOLED("Connected", "Connected");
}

// ================= TASK =================
void TaskExample(void *parameter)
{
  for (;;)
  {
    StaticJsonDocument<256> doc;
    doc["message"] = "Hello world";

    String msg;
    serializeJson(doc, msg);

    mqttPublish(MQTT_Topic, msg);

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);

  // OLED init
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED fail");
    while (1)
      ;
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);

  pinMode(ledPin, OUTPUT);

  WIFIConnect();

  client.setServer(MQTTServer, Port);
  client.setCallback(callback);

  xTaskCreatePinnedToCore(
      TaskExample,
      "TaskExample",
      10000,
      NULL,
      1,
      NULL,
      0);
}

// ================= LOOP =================
void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WIFIConnect();
  }

  if (!client.connected())
  {
    MQTT_Reconnect();
  }

  client.loop();
}