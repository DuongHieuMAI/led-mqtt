#include "mqtt_handler.h"

String ip = "";
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x06};

MQTTClient::MQTTClient() {

}

MQTTClient::~MQTTClient() {

}

void MQTTClient::initEthernet() {
    while (Ethernet.begin(mac) == 0) {
        //Serial.println(F("Unable to configure Ethernet using DHCP"));
        delay(1000);
    }
}
void MQTTClient::initMQTTClient(void (*callback)(char*, uint8_t*, unsigned int)) {
    mqttClient.setClient(ethClient);
    // mqttClient.setServer("ec2-18-136-58-212.ap-southeast-1.compute.amazonaws.com", 1883);//use public broker
    mqttClient.setServer("test.mosquitto.org", 1883);//use public broker
    mqttClient.setCallback(callback);
}

void MQTTClient::subscribe(const char* topic) {
    mqttClient.subscribe(topic);
}

void MQTTClient::publish(const char *topic, const char *payload) {
    mqttClient.publish(topic,payload);
}

void MQTTClient::publishWithRetain(const char *topic, const char *payload, const bool retain) {
    mqttClient.publish(topic,payload,retain);
}

// IPAdress MQTTClient::getLocalIP() {
//     return localIP;
// }

bool MQTTClient::connect() {
    return mqttClient.connect(CLIENT_ID);
}

bool MQTTClient::connectWithWill(const char *willTopic, const char *willMessage) {
    return mqttClient.connect(CLIENT_ID,willTopic,0,true,willMessage);
}

void MQTTClient::loop() {
    mqttClient.loop();
}