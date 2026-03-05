import paho.mqtt.client as mqtt
import time
import json

BROKER = "44.232.241.40"
PORT = 1883
TOPIC = "door/access"

# ================= CALLBACK =================
def on_connect(client, userdata, flags, rc):

    if rc == 0:
        print("Connected to MQTT")

        client.subscribe(TOPIC)

    else:
        print("Connection failed")


def on_message(client, userdata, msg):

    print("Message received")

    print("Topic:", msg.topic)

    data = msg.payload.decode()

    print("Data:", data)


# ================= MQTT CLIENT =================
client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, 60)

client.loop_start()


# ================= SEND JSON =================
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