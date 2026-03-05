# pip install paho-mqtt

import paho.mqtt.client as mqtt
import time
import json

BROKER = "44.232.241.40"
PORT = 1883
TOPIC = "door/access"

client = mqtt.Client()

client.connect(BROKER, PORT, 60)

client.loop_start()

while True:

    data = {
        "name": "Hoa",
        "action": "open",
        "time": "21:30"
    }

    message = json.dumps(data)

    client.publish(TOPIC, message)

    print("Sent:", message)

    time.sleep(5)