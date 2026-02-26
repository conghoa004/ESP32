#include <Arduino.h>

#define BUTTON_INPUT_PIN 4

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(BUTTON_INPUT_PIN, INPUT);
}

void loop()
{
  delay(1000); // Wait for 1 second
}