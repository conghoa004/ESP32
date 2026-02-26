#include <Arduino.h>

#define BUZZER_PIN 18

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{
  digitalWrite(BUZZER_PIN, HIGH); // bật buzzer
  delay(1000);

  digitalWrite(BUZZER_PIN, LOW);  // tắt buzzer
  delay(1000);
}