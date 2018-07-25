#include "mqtt_handler.h" //PubSubClient.h Library from Knolleary
#include "iot_led.h"
#include "ArduinoJson.h"

#define SUBSCRIBE_TOPIC "/user001/home001/room001/led/command"
#define PUBLISH_TOPIC "/user001/home001/room001/led/state"

long previousMillis;
unsigned long long startConnectTime = 0;
long connectTime = 0;

void callback(char* topic, byte* payload, unsigned int length);
MQTTClient mqttClient;

IoTLed iotLed(8,2);
IoTLed iotLed2(9,3);
StaticJsonBuffer<60> jsonBuffer;

void setup() {
    // pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(7, INPUT);

    //setup button input
    // pinMode(2, INPUT);
    pinMode(4, INPUT);
    // pinMode(3, INPUT);

    //set led output
    // pinMode(8, OUTPUT);


    //set up iot led
    iotLed.init();
    iotLed2.init();

    // setup serial communication
    Serial.begin(9600);
    while (!Serial) {};
    Serial.println(F("MQTT Arduino Demo"));
    Serial.println();

    // setup MQTT client
    startConnectTime = millis();
    mqttClient.initEthernet();
    mqttClient.initMQTTClient(callback);
    delay(1000);
    // if(mqttClient.connect()) {
    //   mqttClient.subscribe(SUBSCRIBE_TOPIC);
    // }
    Serial.println("Setting will message");
    StaticJsonBuffer<60> jsonBuffer3;
    JsonObject& root3 = jsonBuffer3.createObject();
    root3["led-1"] = "offline";
    root3["led-2"] = "offline";
    root3["led-3"] = "offline";
    char jsonChar3[60];
    root3.printTo(jsonChar3);
    Serial.println(jsonChar3);
    
    if(mqttClient.connectWithWill(PUBLISH_TOPIC,jsonChar3)) {
      mqttClient.subscribe(SUBSCRIBE_TOPIC);
      // notify the online
      Serial.println("Notify the online");
      StaticJsonBuffer<60> jsonBuffer2;
      JsonObject& root2 = jsonBuffer2.createObject();
      root2["led-1"] = "online";
      root2["led-2"] = "online";
      root2["led-3"] = "online";
      char jsonChar2[60];
      root2.printTo(jsonChar2);
      Serial.println(jsonChar2);
      mqttClient.publishWithRetain(PUBLISH_TOPIC,jsonChar2,true);
    }
    Serial.println("Setup finished!");
    previousMillis = millis();
}

void loop() {
  
  if((millis()-previousMillis)>=200) {
    //200 ms elapsed
    StaticJsonBuffer<50> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    if(iotLed.getLedState()) {
      root["led-1"] = "ON";
      // mqttClient.publish(PUBLISH_TOPIC,"ON");
    }
    else {
      root["led-1"] = "OFF";
      // mqttClient.publish(PUBLISH_TOPIC,"OFF");
    }
    if(iotLed2.getLedState()) {
      root["led-2"] = "ON";
      // mqttClient.publish(PUBLISH_TOPIC,"ON");
    }
    else {
      root["led-2"] = "OFF";
      // mqttClient.publish(PUBLISH_TOPIC,"OFF");
    }    
    char jsonChar[60];
    root.printTo(jsonChar);
    mqttClient.publish(PUBLISH_TOPIC,jsonChar);
    previousMillis = millis();
  }

  //check iot led
  iotLed.loop(); 
  iotLed2.loop();

  mqttClient.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
    char msgBuffer[20];
    // I am only using one ascii character as command, so do not need to take an entire word as payload
    // However, if you want to send full word commands, uncomment the next line and use for string comparison
    // payload[length]='\0';// terminate string with 0
  //String strPayload = String((char*)payload);  // convert to string
    // Serial.println(strPayload); 
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");//MQTT_BROKER
    // for (int i = 0; i < length; i++) {
    //   Serial.print((char)payload[i]);
    // }
    // Serial.println();
    // Serial.println(payload[0]);

    payload[length]='\0';// terminate string with 0
    String strPayload = String((char*)payload);  // convert to string
    Serial.println(strPayload);

    JsonObject& root = jsonBuffer.parseObject(strPayload);
    if (!root.success()) {
        Serial.println("JSON: parseObject() failed");
        
        return;
    }
    const char* ledSetState = root["led-1"];
    Serial.print("led-1 command: ");
    // Serial.println(ledSetState);
    // ledSetState[length] = '\0';
    String ledSetStateStr = String((char*)ledSetState);
    // Serial.println(ledSetStateStr);
    
    if(!ledSetStateStr.compareTo("ON")){
      Serial.println("Command ON");
      iotLed.setLedState(true);
    } else if(!ledSetStateStr.compareTo("OFF")){
      Serial.println("Command OFF");
      iotLed.setLedState(false);
    } else {
      Serial.println("Command unknown");
    }
}