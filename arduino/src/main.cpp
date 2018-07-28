#include "mqtt_handler.h" //PubSubClient.h Library from Knolleary
#include "iot_led.h"
#include "ArduinoJson.h"
// #include "vector.h"

#define SUBSCRIBE_TOPIC "/user001/home001/room001/led/command"
#define PUBLISH_TOPIC "/user001/home001/room001/led/state"

long previousMillis;
// unsigned long long startConnectTime = 0;
// long connectTime = 0;

void callback(char* topic, byte* payload, unsigned int length);
MQTTClient mqttClient;

// IoTLed iotLed("led-1",8,2);
// IoTLed iotLed2("led-2",9,3);
const byte led_num = 2;
IoTLed arrayIoTLed[led_num] {{"led-1",8,2},{"led-2",9,3}};
// std::vector<IoTLed> ledList;

// StaticJsonBuffer<60> jsonBufferCallback;


void connectMQTT();

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

    // setup serial communication
    Serial.begin(9600);
    while (!Serial) {};
    // Serial.println(F("Serial comm done."));
    Serial.println(F("===MQTT Arduino Demo==="));
    // Serial.println();

    //set up iot led
    Serial.println("Setting leds...");
    for(byte i =0; i< led_num; i++){
      arrayIoTLed[i].init();
    }
    // Serial.println("Getting led name...");
    // for(byte i =0; i< led_num; i++){
      // Serial.print("IoTLed ");
      // Serial.print(i+1);
      // Serial.print(" : ");
      // Serial.println(arrayIoTLed[i].getLedName());
    // }
    Serial.println("Setting leds done");

    // setup MQTT client
    do {
      Serial.println("Trying to connect to broker...");
      connectMQTT();
    } while (!mqttClient.connected());
    Serial.println("Connected to broker");
    Serial.println("-----------------------------------");
    delay(1000);
    previousMillis = millis();
}

void loop() {

  if(mqttClient.connected()) {
    StaticJsonBuffer<60> jsonBufferGlobal;
    char jsonCharLoop[60];
  
    if((millis()-previousMillis)>=250) {
      //200 ms elapsed
      JsonObject& rootLoop = jsonBufferGlobal.createObject();

      for(byte i =0;i<led_num;i++) {
        if(arrayIoTLed[i].getLedState()) {
          rootLoop[arrayIoTLed[i].getLedName()] = "ON";
        } else {
          rootLoop[arrayIoTLed[i].getLedName()] = "OFF";
        }
      }
      
      rootLoop.printTo(jsonCharLoop);
      // char *jsonData = "jsonCharLoop";
      // Serial.println(jsonCharLoop);
      if (mqttClient.publish(PUBLISH_TOPIC,jsonCharLoop) == true) {
        Serial.print("Published MQTT msg: ");      
      } else {
        Serial.print("Publish MQTT msg failed: ");
      }
      Serial.println(jsonCharLoop);
      jsonBufferGlobal.clear();
      // rootLoop.
      // jsonCharLoop
      mqttClient.loop();
      previousMillis = millis();
    }   
    // mqttClient.loop();
  } else {    
    do {
      Serial.println("Disconected to broker. Reconnecting...");
      connectMQTT();
    } while(!mqttClient.connected());
  }
   //check iot led
  for(byte i =0;i<led_num;i++) {
    arrayIoTLed[i].loop();
  }  
}

void callback(char* topic, byte* payload, unsigned int length) {
    // char msgBuffer[20];
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

    StaticJsonBuffer<60> jsonBufferCallback;
    JsonObject& root = jsonBufferCallback.parseObject(strPayload);
    if (!root.success()) {
        Serial.println("JSON: parseObject() failed");        
        return;
    }

    // const char* ledSetStateAll = root["led-all"];
    JsonVariant ledSetStateAll = root["led-all"];
    if(ledSetStateAll.success())
    {
      // Serial.println(ledSetStateAll.as<char*>());
      String ledSetStateAllStr = String(ledSetStateAll.as<String>());
      // String ledSetStateAllStr;
      // ledSetStateAll.printTo(ledSetStateAllStr);
      // char temp[10] ="";
      // ledSetStateAll.printTo(temp);
      // Serial.println(temp);
      // String ledSetStateAllStr = String((char*)temp);
      Serial.print("Command for all leds: ");
      Serial.println(ledSetStateAllStr);
      Serial.println(ledSetStateAllStr.length());
      // String on("ON");
      // Serial.println(on.length());
      if(!ledSetStateAllStr.compareTo("ON")){        
        for(byte i =0;i<led_num;i++){
          arrayIoTLed[i].setLedState(true);        
        }
        return; 
      } else if(!ledSetStateAllStr.compareTo("OFF")){
        for(byte i =0;i<led_num;i++){
          arrayIoTLed[i].setLedState(false);
        }
        return;
      } else {
        Serial.println("Command for all unknown!");
      }
    } else {

      for(byte i =0;i<led_num;i++)
      {
        JsonVariant ledSetState = root[arrayIoTLed[i].getLedName()];

        if(ledSetState.success())
        {        

          int a;
          a = 8;
          Serial.println(a);

          // String ledSetStateStr = String(ledSetState.as<String>());
          // Serial.println(ledSetStateStr.length());
        //   Serial.print(arrayIoTLed[i].getLedName());
        //   Serial.print(" got command: ");
        //   Serial.println(ledSetStateStr);

          // if(!ledSetStateStr.compareTo("ON"))
          // {
          //   arrayIoTLed[i].setLedState(true);
          // } else if(!ledSetStateStr.compareTo("OFF")){
          //   arrayIoTLed[i].setLedState(false);
          // } else {
          //   Serial.println("Command unknown");
          // }
        }
      }
    }
    jsonBufferCallback.clear();
}

void connectMQTT() {

    Serial.println("Setting MQTT...");
    // startConnectTime = millis();
    mqttClient.initEthernet();
    mqttClient.initMQTTClient(callback);
    delay(1000);
    // if(mqttClient.connect()) {
    //   mqttClient.subscribe(SUBSCRIBE_TOPIC);
    // }
    Serial.println("Setting will message...");
    StaticJsonBuffer<60> jsonBufferSettingMqtt;
    JsonObject& rootWillMessage = jsonBufferSettingMqtt.createObject();
    for (byte i =0;i<led_num;i++) {
      rootWillMessage[arrayIoTLed[i].getLedName()] = "offline";
    }
    char jsonChar3[60];
    rootWillMessage.printTo(jsonChar3);
    Serial.println(jsonChar3);
    jsonBufferSettingMqtt.clear();
    
    // if(mqttClient.connectWithWill(PUBLISH_TOPIC,jsonChar3)) {
    //   mqttClient.subscribe(SUBSCRIBE_TOPIC);
    //   // notify the online
    //   Serial.println("Notify the online");
    //   StaticJsonBuffer<60> jsonBuffer2;
    //   JsonObject& root2 = jsonBuffer2.createObject();
    //   for(byte i =0;i<led_num;i++){
    //     root2[arrayIoTLed[i].getLedName()] = "online";
    //   }
    //   char jsonChar2[60];
    //   root2.printTo(jsonChar2);
      
    //   Serial.println(jsonChar2);
    //   mqttClient.publishWithRetain(PUBLISH_TOPIC,jsonChar2,true);
    // }

    if(mqttClient.connectWithWill(PUBLISH_TOPIC,jsonChar3)) {
      mqttClient.subscribe(SUBSCRIBE_TOPIC);
      // notify the online
      Serial.println("Notify the online");
      // StaticJsonBuffer<60> jsonBuffer2;
      JsonObject& rootOnlineMessage = jsonBufferSettingMqtt.createObject();
      for(byte i =0;i<led_num;i++){
        rootOnlineMessage[arrayIoTLed[i].getLedName()] = "online";
      }
      char jsonChar2[60];
      rootOnlineMessage.printTo(jsonChar2);
      jsonBufferSettingMqtt.clear();
      
      Serial.println(jsonChar2);
      mqttClient.publishWithRetain(PUBLISH_TOPIC,jsonChar2,true);
    } else {
      Serial.println("Can not connect to broker!");
    }

    Serial.println("Setup MQTT finished!");
}