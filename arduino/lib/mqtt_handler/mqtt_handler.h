#ifndef mqtt_hander_h
#define mqtt_hander_h

#include "Ethernet.h"// Ethernet.h library
#include "PubSubClient.h" //PubSubClient.h Library from Knolleary
#include "Arduino.h"
#include "IPAddress.h"

#define CLIENT_ID       "Hal"
#define PUBLISH_DELAY   3000 // that is 3 seconds interval

class MQTTClient {

    public:

    MQTTClient();
    ~MQTTClient();
    void initEthernet();
    void initMQTTClient(void (*callback)(char*, uint8_t*, unsigned int));
    void subscribe(const char* topic);
    void publish(const char *topic, const char *payload);
    void publishWithRetain(const char *topic, const char *payload, const bool retain);
    void loop();
    bool connect();
    bool connectWithWill(const char *willTopic, const char *willMessage);
    // IPAddress getLocalIP();
    
    private:

    EthernetClient ethClient;
    PubSubClient mqttClient;
    // IPAddress localIP;

    // method
    // void callback(char* topic, byte* payload, unsigned int length);
    // boolean publish(const char* topic, const char* payload);
};
#endif