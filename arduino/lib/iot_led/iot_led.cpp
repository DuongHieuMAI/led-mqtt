#include "iot_led.h"

IoTLed::IoTLed(const int ledPin, const int buttonPin) {
    this->ledPin = ledPin;
    this->buttonPin = buttonPin;
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