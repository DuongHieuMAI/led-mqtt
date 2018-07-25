package com.example.engineer.led_control;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.app.TaskStackBuilder;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOError;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Iterator;

public class MainActivity extends AppCompatActivity //implements MqttMessageService.OnMessageReceive { TODO: check
{
    private MqttAndroidClient client;
    private String TAG = "MainActivity";
    private MQTTClient mqttClient;

    private Button buttonSubscribe;
    private Button buttonExit;


    private class IoTButton {
        public String keyName;
        public Button button;

        public void setup( Button button, String keyName) {
            this.button = button;
            this.keyName = keyName;
        }
    }

    private IoTButton[] buttonList = new IoTButton[3];


    /*TODO: Check this method
    private MqttMessageService mqttMessageService;
    */


    private BroadcastReceiver br = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Bundle bundle = intent.getExtras();
            if(bundle!=null) {
                Log.d(TAG,"Activity got data from Service");
                String receivedData = bundle.getString(Constants.SUBSCRIBE_TOPIC);
                Log.d(TAG,"Data got: " + receivedData);
                //process message
                processReceivedData(receivedData);
            }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mqttClient = new MQTTClient();
        client = mqttClient.getMqttClient(getApplicationContext(),Constants.MQTT_BROKER_URL, Constants.CLIENT_ID);

        Button buttonLed1 = (Button) findViewById(R.id.buttonLed1);
        Button buttonLed2 = (Button) findViewById(R.id.buttonLed2);
        Button buttonLed3 = (Button) findViewById(R.id.buttonLed3);
        buttonSubscribe = (Button)findViewById(R.id.buttonSubscribe);
        buttonExit = (Button)findViewById(R.id.buttonExit);
        final Button buttonAll = (Button) findViewById(R.id.buttonLedAll);



        IoTButton firstOne = new IoTButton();
        firstOne.setup(buttonLed1,"led-1");
        buttonList[0] = firstOne;


        IoTButton secondOne = new IoTButton();
        secondOne.setup(buttonLed2,"led-2");
        buttonList[1] = secondOne;


        IoTButton thirdOne = new IoTButton();
        thirdOne.setup(buttonLed3,"led-3");
        buttonList[2] = thirdOne;

        setupButtonLed(buttonList[0].button,buttonList[0].keyName);
        setupButtonLed(buttonList[1].button,buttonList[1].keyName);
        setupButtonLed(buttonList[2].button,buttonList[2].keyName);

        //Hide
//        buttonSubscribe.setVisibility(View.INVISIBLE);

        // Subscribe to topic
        buttonSubscribe.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String buttonText = buttonSubscribe.getText().toString();
                Log.d(TAG,buttonText);
                if (buttonText.equals("subscribe")) {
                    Log.d(TAG,"Press Subscribe button");
                    buttonSubscribe.setText("unsubscribe");
                    try {
                        mqttClient.subscribe(client,Constants.SUBSCRIBE_TOPIC,1);
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                } else if (buttonText.equals("unsubscribe")) {
                    Log.d(TAG,"Press Unsubscribe button");
                    buttonSubscribe.setText("subscribe");
                    try {
                        mqttClient.unSubscribe(client,Constants.SUBSCRIBE_TOPIC);
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                } else {
                    Log.e(TAG,"Can not determine action by button's text");
                }
            }
        });

        buttonAll.setText("led-all: on");
        buttonAll.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String buttonText = buttonAll.getText().toString();
                Log.d(TAG,buttonText);
                if(buttonText.equals("led-all: on")) {
                    Log.d(TAG, "Turn on all leds");
                    JSONObject jsonData = new JSONObject();
                    try {
                        jsonData.put("led-all","ON");
                        String sendingData = jsonData.toString();
                        mqttClient.publishMessage(client,Constants.PUBLISH_TOPIC,sendingData,1);
                        buttonAll.setText("led-all: off");
                        Log.d(TAG,"Publish command ON");
                    } catch (MqttException e) {
                        e.printStackTrace();
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                } else if(buttonText.equals("led-all: off")) {
                    Log.d(TAG, "Turn off all leds");
                    JSONObject jsonData = new JSONObject();
                    try {
                        jsonData.put("led-all","OFF");
                        String sendingData = jsonData.toString();
                        mqttClient.publishMessage(client,Constants.PUBLISH_TOPIC,sendingData,1);
                        buttonAll.setText("led-all: on");
                        Log.d(TAG,"Publish command OFF");
                    } catch (MqttException e) {
                        e.printStackTrace();
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        });

        buttonExit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    mqttClient.unSubscribe(client,Constants.SUBSCRIBE_TOPIC);
                } catch (MqttException e) {
                    e.printStackTrace();
                }
                finish();
            }
        });

        Intent intent = new Intent(MainActivity.this, MqttMessageService.class);
        startService(intent);
        /* TODO: Check this method if binding is better broadcast receiver
        bindService(intent, new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
                if (iBinder instanceof MqttMessageService.MyOwnBinder) {
                    mqttMessageService = ((MqttMessageService.MyOwnBinder) iBinder).getMyService();
                    mqttMessageService.setMessageReceive(MainActivity.this);
                }
            }

            @Override
            public void onServiceDisconnected(ComponentName componentName) {

            }
        }, Context.BIND_AUTO_CREATE);
        */
    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(br,new IntentFilter("MQTT_data"));
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(br);
    }

    private void processReceivedData(String receivedData) {
        String state = "";
        try {
            JSONObject jsonData = new JSONObject(receivedData);
            if (jsonData.length() == 0) {
                Log.e(TAG, "json data empty");
            }
            Iterator<String> iter = jsonData.keys();
            while (iter.hasNext()) {
                String key = iter.next();
                for (int i = 0; i < buttonList.length; i++) {
                    if(key.equals(buttonList[i].keyName)) {
                        try {
                            state = jsonData.getString(key);
                            Log.d(TAG,"State got for" + key);
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                        if(state.equals("online")) {
                            buttonList[i].button.setEnabled(true);
                            buttonList[i].button.setText(": "+state);
                        } else if (state.equals("ON")) {
                            buttonList[i].button.setText(buttonList[i].keyName+": OFF");
                            buttonList[i].button.setBackgroundColor(Color.YELLOW);
                        } else if (state.equals("OFF")) {
                            buttonList[i].button.setText(buttonList[i].keyName+ ": ON");
                            buttonList[i].button.setBackgroundColor(Color.GREEN);
                        } else  if(state.equals("offline")) {
                            buttonList[i].button.setText(buttonList[i].keyName+": "+state);
                            buttonList[i].button.setEnabled(false);
                        }
                    }
                }
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

    }

    /* TODO: Check this method
    @Override
    public void onMessageReceive(String message) {

    }
    */

    private void setupButtonLed(final Button button, final String keyname) {
        button.setEnabled(false);
        button.setText(keyname + ": offline");
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String msg = button.getText().toString();
                if (msg.equals(keyname +": ON")) {
                    JSONObject jsonData = new JSONObject();
                    try {
                        jsonData.put(keyname,"ON");
                        String sendingData = jsonData.toString();
                        mqttClient.publishMessage(client,Constants.PUBLISH_TOPIC,sendingData,1);
                        Log.d(TAG,"Publish command ON");
                    } catch (MqttException e) {
                        e.printStackTrace();
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                } else if(msg.equals(keyname + ": OFF")) {
                    JSONObject jsonData = new JSONObject();
                    try {
                        jsonData.put(keyname,"OFF");
                        String sendingData = jsonData.toString();
                        mqttClient.publishMessage(client,Constants.PUBLISH_TOPIC,sendingData,1);
                        Log.d(TAG,"Publish command OFF");
                    } catch (MqttException e) {
                        e.printStackTrace();
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                } else if(msg.equals(keyname + ": online")) {
                    Log.d(TAG,"Please wait for updating state");
                }
                else {
                    Log.e(TAG,"invalid action");
                }
            }
        });
    }
}
