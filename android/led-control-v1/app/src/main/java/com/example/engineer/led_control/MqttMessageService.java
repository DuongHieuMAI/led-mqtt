package com.example.engineer.led_control;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.app.TaskStackBuilder;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.Serializable;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;

public class MqttMessageService extends Service {

    private static final String TAG = "MqttMessageService";
    private MQTTClient mqttClient;
    private MqttAndroidClient mqttAndroidClient;
    private MyOwnBinder myOwnBinder;
    private OnMessageReceive messageReceive;

    public MqttMessageService() {
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG,"onCreate");
        mqttClient = new MQTTClient();
        mqttAndroidClient = mqttClient.getMqttClient(getApplicationContext(),Constants.MQTT_BROKER_URL,Constants.CLIENT_ID);

        mqttAndroidClient.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {

            }

            @Override
            public void connectionLost(Throwable throwable) {

            }

            @Override
            public void messageArrived(String s, MqttMessage mqttMessage) throws Exception {
                setMessageNotification(s, new String(mqttMessage.toString()));
                publishData(s,new String((mqttMessage.toString())));
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

            }
        });
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG,"onStartCommand");
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service. Check this
        if (myOwnBinder == null) {
            myOwnBinder = new MyOwnBinder();
        }
        return myOwnBinder;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG,"onDestroy");
    }

    public void setMessageReceive(OnMessageReceive messageReceive) {
        this.messageReceive = messageReceive;
    }

    private void setMessageNotification(@NonNull String topic, @NonNull String msg) {
        Log.d(TAG,"Message came!");
//        NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(this, "notify_id")
        NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(this)
                .setSmallIcon(R.drawable.ic_launcher_foreground)
                .setContentTitle(topic)
                .setContentText(msg);
        Intent resultIntent = new Intent(this, MainActivity.class);

        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addParentStack(MainActivity.class);
        stackBuilder.addNextIntent(resultIntent);
        PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);
        mBuilder.setContentIntent(resultPendingIntent);
        NotificationManager mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.notify(100, mBuilder.build());
    }

    private void publishData(@NonNull String topic, @NonNull String msg) {
        Intent intent = new Intent("MQTT_data");
        intent.putExtra(topic,msg);
        sendBroadcast(intent);
        Log.d(TAG,"Sent data to activity");
        /*
        if (messageReceive != null) {
            messageReceive.onMessageReceive(msg);
        }
        */
    }

    class MyOwnBinder extends Binder {
        MqttMessageService getMyService() {
            return MqttMessageService.this;
        }
    }

    interface OnMessageReceive {
        void onMessageReceive(String message);
    }
}
