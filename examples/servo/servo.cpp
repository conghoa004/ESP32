#include <Arduino.h>
#include <ESP32Servo.h>

const int servoPin = 18;
Servo servo;
int pos = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  servo.attach(servoPin, 500, 2400);
}

void loop()
{
 for (pos = 0; pos <= 180; pos += 1) {
    servo.write(pos);
    delay(15);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    servo.write(pos);
    delay(15);
  }
}