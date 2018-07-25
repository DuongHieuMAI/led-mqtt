import paho.mqtt.client as mqtt
import json
import time

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
<<<<<<< HEAD
    client.subscribe("/user001/home001/room003/led/state")
=======
    client.subscribe("/user001/home001/room001/led/state")
>>>>>>> master

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

<<<<<<< HEAD
client.connect("test.mosquitto.org", 1883, 60)
=======
client.connect("ec2-18-136-58-212.ap-southeast-1.compute.amazonaws.com", 1883, 60)
>>>>>>> master

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
while True:
    client.loop()
    # time.sleep(3)

