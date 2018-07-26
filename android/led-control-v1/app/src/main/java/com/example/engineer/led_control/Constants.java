package com.example.engineer.led_control;

public class Constants {
    public static final String MQTT_BROKER_URL = "tcp://ec2-18-136-58-212.ap-southeast-1.compute.amazonaws.com:1883";
    public static final String PUBLISH_TOPIC = "/user001/home001/room001/led/command";
    public static final String SUBSCRIBE_TOPIC = "/user001/home001/room001/led/state";
    public static final String CLIENT_ID = "user001";
}
