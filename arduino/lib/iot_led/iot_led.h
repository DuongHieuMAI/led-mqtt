#ifndef iot_led_h
#define iot_led_h

#include "Arduino.h"

class IoTLed {
    public:
    IoTLed();
    IoTLed(const char *ledName, const int ledPin, const int buttonPin);
    ~IoTLed();
    void init();
    void loop();
    void setLedState(const bool ledState);
    bool getLedState();
    // bool getLedStateInStr();
    char* getLedName();
    void setLedName(const char *ledName);


    private:
    int buttonPin;
    int ledPin;
    char *ledName;
    bool buttonPress;
    bool ledStatus;
    
};

#endif