#include "iot_led.h"
IoTLed::IoTLed() {

}

IoTLed::IoTLed(const char *ledName, const int ledPin, const int buttonPin) {
    this->ledPin = ledPin;
    this->buttonPin = buttonPin;
    this->ledName = ledName;
    buttonPress = false;
    ledStatus = false;
}

IoTLed::~IoTLed() {

}

void IoTLed::init() {
    pinMode(ledPin,OUTPUT);
    pinMode(buttonPin,INPUT);
    digitalWrite(ledPin,false);
}

void IoTLed::loop() {
    if (!digitalRead(buttonPin)) {
        if(!buttonPress) {
            buttonPress = true;
            // Check led status            
            ledStatus = !ledStatus;
            digitalWrite(ledPin,ledStatus);
        }    
    } else if (buttonPress) {
        // Serial.println("Button 2 release");
        buttonPress = false;
    }
}

void IoTLed::setLedState(const bool ledState) {
    ledStatus = ledState;
    digitalWrite(ledPin,ledStatus);
}
bool IoTLed::getLedState() {
    return ledStatus;
}

char* IoTLed::getLedName() {
    return ledName;    
}

void IoTLed::setLedName(const char *ledName) {
    this->ledName = ledName;
}