#ifndef iot_led_h
#define iot_led_h

#include "Arduino.h"

class IoTLed {
    public:
    IoTLed(const int ledPin, const int buttonPin);
    ~IoTLed();
    void init();
    void loop();
    void setLedState(const bool ledState);
    bool getLedState();


    private:
    int buttonPin;
    int ledPin;
    bool buttonPress;
    bool ledStatus;
};

#endif