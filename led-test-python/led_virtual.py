import paho.mqtt.client as mqtt
import json

SUBSCRIBE_TOPIC = "/user001/home001/room001/led/command"
PUBLISH_TOPIC = "/user001/home001/room001/led/state"
import time

stateLed1 = 'ON'
stateLed2 = 'ON'
stateLed3 = 'ON'
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    
    #notify the avaibility
    state = json.dumps({
        "led-1": "online",
        "led-2": "online",
        "led-3": "online"
    })

    client.publish(PUBLISH_TOPIC,state,retain=True)

    client.subscribe(SUBSCRIBE_TOPIC)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    if(msg.topic==SUBSCRIBE_TOPIC):
        dict = json.loads(msg.payload)
        global stateLed1, stateLed2, stateLed3
        # print dict
        if "led-all" not in dict:
            if "led-1" in dict:    
                if dict['led-1'] == "ON" :
                    stateLed1 = 'ON'
                elif dict['led-1'] == "OFF" :
                    stateLed1 = "OFF"
                else:
                    print "Invalid Command for led 1"

            if "led-2" in dict :       
                if dict['led-2'] == "ON" :
                    stateLed2 = 'ON'
                elif dict['led-2'] == "OFF" :
                    stateLed2 = "OFF"
                else:
                    print "Invalid Command for led-2"

            if "led-3" in dict:
                if dict['led-3'] == "ON" :
                    stateLed3 = 'ON'
                elif dict['led-3'] == "OFF" :
                    stateLed3 = "OFF"
                else:
                    print "Invalid Command for led-3"

        elif dict['led-all'] == "ON" :
            stateLed1 = 'ON'
            stateLed2 = stateLed3 = stateLed1
        elif dict['led-all'] == "OFF" :
            stateLed1 = "OFF"
            stateLed2 = stateLed3 = stateLed1
        else:
            print "Invalid Command for led"

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
will_payload = json.dumps({
    "led-1":"offline",
    "led-2":"offline",
    "led-3":"offline"
})
client.will_set(PUBLISH_TOPIC,will_payload,retain=True)

client.connect("test.mosquitto.org", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
# client.loop_forever()
client.loop_start()
while True:
    j = json.dumps({
        "led-1": stateLed1,
        "led-2": stateLed2,
        "led-3": stateLed3
    })
    # print j
    client.publish(PUBLISH_TOPIC,j)
    # client.loop();
    time.sleep(0.5)