/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Ian Craggs - make sure QoS2 processing works, and add device headers
 *******************************************************************************/

 /**
  This is a sample program to illustrate the use of the MQTT Client library
  on the mbed platform.  The Client class requires two classes which mediate
  access to system interfaces for networking and timing.  As long as these two
  classes provide the required public programming interfaces, it does not matter
  what facilities they use underneath. In this program, they use the mbed
  system libraries.

 */

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#define MQTTCLIENT_QOS1 0
#define MQTTCLIENT_QOS2 0

#include "mbed.h"
#include "NTPClient.h"
#include "TLSSocket.h"
#include "MQTTClientMbedOs.h"
#include "MQTT_server_setting.h"
#include "mbed-trace/mbed_trace.h"
#include "mbed_events.h"
#include "mbedtls/error.h"

//my includes
#include "Dht21.h"
#include "Queues.h"
#include "functions.h"
#include "HandleMessages.h"
#include <string>

#define LED_ON  MBED_CONF_APP_LED_ON
#define LED_OFF MBED_CONF_APP_LED_OFF

/* Flag to be set when received a message from the server. */
static volatile bool isMessageArrived = false;
static volatile bool isPublish = false;
/* Buffer size for a receiving message. */
const int MESSAGE_BUFFER_SIZE = 256;
/* Buffer for a receiving message. */
char messageBuffer[MESSAGE_BUFFER_SIZE];
volatile uint64_t pulses=0;

Thread tempControlThread(osPriorityNormal,2048);
Thread groundControlThread(osPriorityNormal,2048);
Thread ledPanelControlThread(osPriorityNormal,2048);

MemoryPool<message_t, 16> mpool;
Queue<message_t, 16> queue;

MemoryPool<message_l, 16> mpoolLed;
Queue<message_l, 16> queueLed;

MemoryPool<message_g, 16> mpoolGround;
Queue<message_g,16> queueGround;

/*
 * Callback function called when a message arrived from server.
 */
void messageArrived(MQTT::MessageData& md)
{
    // Copy payload to the buffer.
    MQTT::Message &message = md.message;
    if(message.payloadlen >= MESSAGE_BUFFER_SIZE) {
        // TODO: handling error
    } else {
        memcpy(messageBuffer, message.payload, message.payloadlen);
    }
    messageBuffer[message.payloadlen] = '\0';

    isMessageArrived = true;
}
void publishMessage(string message);
/*
 * Callback function called when the button1 is clicked.
 */
void btn1_rise_handler() {
    isPublish=!isPublish;
}
void pulseCounter(){
    pulses++;
}
double getMililiters(){
    return double(pulses)/9.3; //5880 pulses per liter
}

void tempControl(){
    Dht21 sensorIn(D4);
    Dht21 sensorOut(D3);
    DigitalOut fans(D11);
    double tempIn,humIn,tempOut,humOut;
    uint8_t inFaultCtr = 0;
    uint8_t outFaultCtr = 0;
    uint8_t state = 0;
    uint8_t hours,minutes,seconds;
    uint64_t actualTime;
    bool tempControl = 0;
    bool humControl = 0;
    fans = 0;
    uint8_t counter = 0;
    bool turnedOff = 0;
    while(1){
    thread_sleep_for(2000);  
    actualTime = upTime(hours, minutes, seconds);
    switch (state) {
        case 0://Pomiar
        {
            uint8_t updateIn = sensorIn.read();
            uint8_t updateOut  = sensorOut.read();  
            if(updateIn != 0)
                ++inFaultCtr;
            else if (updateOut != 0)
                ++outFaultCtr;
            else {
                tempIn = sensorIn.getCelsius();
                humIn = sensorIn.getHumidity();
                tempOut = sensorOut.getCelsius();
                humOut = sensorOut.getHumidity(); 
                outFaultCtr = 0;
                inFaultCtr = 0;
                counter++;
                if(counter % 10 == 0 || fans == 1 || turnedOff) 
                    tempControlQueue(tempIn,humIn,tempOut,humOut,(bool)fans,actualTime,&queue,&mpool); 
                if(turnedOff)
                    turnedOff = 0;
                if(counter >= 10)
                    counter = 0;
                state = 1;
            }
            if(inFaultCtr > 30 || outFaultCtr > 30){
                state = 2;
                tempControlQueue(0, 0, 0, 0, 0, 0, &queue, &mpool);
            }

            break;
        }  

        case 1://Sterowanie
        { 
        if(tempIn > 21 && humControl == 0 && tempControl == 0){
            if(tempIn - tempOut > 2.5){
                tempControl = 1;
                fans = 1;
                printf("TON# %.2f,%.2f,%.2f,%.2f,1,0,1\n",tempIn,tempOut,humIn,humOut);
            }
        }   
        if(tempControl){
            if(tempIn - tempOut < 2){
                tempControl = 0;
                fans = 0;
                turnedOff = 1;
                printf("TOFF# %.2f,%.2f,%.2f,%.2f,0,0,0 \n",tempIn,tempOut,humIn,humOut);
            }
        }
        if(tempIn > 21 && humOut > 35 && !tempControl && !humControl){
            if(humIn - humOut > 6){
                humControl = 1;
                fans = 1;
                printf("HON# %.2f,%.2f,%.2f,%.2f,0,1,1 \n",tempIn,tempOut,humIn,humOut);
            }
        }
        if(humControl){
            if(humIn - humOut < 5){
                humControl = 0;
                fans = 0;
                turnedOff = 1;
                printf("HOFF# %.2f,%.2f,%.2f,%.2f,0,0,0\n",tempIn,tempOut,humIn,humOut);
            }
        }
        state = 0;
        break;
        }
        default://Bledy
        {
            printf("Temp Control Task \n");
            state = 2;
            fans = 0;
            thread_sleep_for(1000 * 60 * 10);
        }
        break;
        }
    } 
    
}

void ledPanelControl(){
    DigitalOut ledPanel(D12);
    DigitalOut fanMix(D6);//fanMix.period(1/20.0f); float dutyCycle = 0.7f;
    uint8_t ledPanelHourStart = 6;
    uint8_t growTime = 14;
    uint64_t t;
    ledPanel = 0;
    fanMix = 0;
    while(1){
        uint8_t hours,minutes,seconds;
        uint64_t actualTime = upTime(hours,minutes,seconds);
        bool statusChanged = 0;
        if(hours < (ledPanelHourStart+growTime)%24 && hours >= ledPanelHourStart){
            if(ledPanel == 0){
                ledPanel = 1;
                statusChanged = 1;
                printf("Start Led Panel %d:%d:%d \n",hours,minutes,seconds);
            }
        } else {
            if(ledPanel == 1){
                ledPanel = 0;
                statusChanged = 1;
                printf("Stop Led Panel %d:%d:%d \n",hours,minutes,seconds);
            }
        }
        if (hours % 2 == 0){
            if(fanMix == 0){
                fanMix = 1;
                statusChanged = 1;
                printf("FanMix Start \n");
            }
        } else {
            if(fanMix == 1){
                fanMix = 0;
                statusChanged = 1;
                printf("FanMix Stop \n");            
            }
        } 
        if(statusChanged)
            ledControlQueue(double(ledPanel), double(fanMix),double(ledPanelHourStart),double(growTime),actualTime,&queueLed,&mpoolLed);
        thread_sleep_for(1000);
    }
}
void groundControl(){
    DigitalOut pump(D7);
    AnalogIn ground_sensor(A0);
    InterruptIn water_sensor(D2);
    water_sensor.rise(&pulseCounter);
    bool printMes = 1;
    uint8_t state = 0;
    uint8_t timeCounter = 0;
    double totalMil = 0.0;
    uint16_t cycles = 0;
    uint16_t BLINKING_RATE = 1000;
    uint8_t hours,minutes,seconds;
    uint64_t startTime;
    while(1){
        double voltage = (3.3/pow(2, 16)) * ground_sensor.read_u16();
        uint64_t actualTime = upTime(hours,minutes,seconds);  
        switch (state) {
        case 0:
            if(voltage > 2){
                pump = 1;
                state = 1;
                totalMil = getMililiters();
                startTime = upTime(hours,minutes,seconds);
                if(printMes)    printf("Voltage is %.2f, turing pump ON \n",voltage);
            }
        break;

        case 1:
            if(cycles % 5 == 0)
                if(printMes)    printf("Actual voltage:%.2f, still pumping %llu\n",voltage,actualTime);
            if(voltage < 1.5){
                pump = 0;
                state = 0;
                timeCounter = 0;
                double oneCycleUsage = getMililiters() - totalMil;
                GroundControlQueue(oneCycleUsage, totalMil, voltage, timeCounter , actualTime, &mpoolGround, &queueGround);
                if(printMes)    printf("Voltage under 2.0V (%.2f), turing pump OFF, water usage %.2f milliliters %llu\n",voltage,oneCycleUsage,actualTime);
            }else{
                if(timeCounter>60){
                    state = 2;
                    double isIncreased = totalMil - getMililiters();        
                    if(!isIncreased){
                        if(printMes)    printf("Mililiters same value, pump Error tMil=%.2f, actualValue=%.2f %llu\n",totalMil,getMililiters(),actualTime);
                        GroundControlQueue(0, 0, 0, 0, 0, &mpoolGround, &queueGround);
                    }else{
                        if(printMes)    printf("Ground sensor error \n");
                        GroundControlQueue(1, 1, 1, 1, 1, &mpoolGround, &queueGround);
                    }
                    pump = 0;
                    continue; 
                }
                timeCounter++;
            }  
        break;

        case 2:
            if(printMes)    printf("GroundControlTask \n");       
            thread_sleep_for(BLINKING_RATE*100);
        break;
        default:
            if(printMes)    printf("Ground Control Task default \n");
        break;
        }

        cycles++;
        if(cycles % 300000 == 0){
            printf("Ground sensor value =%.2f %llu\n",voltage,actualTime);
            cycles = 0;
        }
        thread_sleep_for(BLINKING_RATE);
    }
}
int main(int argc, char* argv[])
{
    thread_sleep_for(100);
    mbed_trace_init();
    const float version = 1.0;
    bool isSubscribed = false;

    NetworkInterface* network = NULL;
    TLSSocket *socket = new TLSSocket; // Allocate on heap to avoid stack overflow.
    MQTTClient* mqttClient = NULL;

    DigitalOut led(MBED_CONF_APP_LED_PIN, LED_ON);

    printf("HelloMQTT: version is %.2f\r\n", version);
    printf("\r\n");

    #ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif

    printf("Opening network interface...\r\n");
    {
        network = NetworkInterface::get_default_instance();
        if (!network) {
            printf("Error! No network inteface found.\n");
            return -1;
        }

        printf("Connecting to network\n");
        nsapi_size_or_error_t ret = network->connect();
        if (ret) {
            printf("Unable to connect! returned %d\n", ret);
            return -1;
        }
    }
    printf("Network interface opened successfully.\r\n");
    printf("\r\n");

    // sync the real time clock (RTC)
    NTPClient ntp(network);
    ntp.set_server("time.google.com", 123);
    time_t now = ntp.get_timestamp()+7200; //2 hours offset
    printf("now=%d \r\n" ,now);
    set_time(now);
    printf("Time is now %s", ctime(&now));

    printf("Connecting to host %s:%d ...\r\n", MQTT_SERVER_HOST_NAME, MQTT_SERVER_PORT);
    {
        nsapi_error_t ret = socket->open(network);
        if (ret != NSAPI_ERROR_OK) {
            printf("Could not open socket! Returned %d\n", ret);
            return -1;
        }
        ret = socket->set_root_ca_cert(SSL_CA_PEM);
        if (ret != NSAPI_ERROR_OK) {
            printf("Could not set ca cert! Returned %d\n", ret);
            return -1;
        }
        ret = socket->set_client_cert_key(SSL_CLIENT_CERT_PEM, SSL_CLIENT_PRIVATE_KEY_PEM);
        if (ret != NSAPI_ERROR_OK) {
            printf("Could not set keys! Returned %d\n", ret);
            return -1;
        }
        ret = socket->connect(MQTT_SERVER_HOST_NAME, MQTT_SERVER_PORT);
        if (ret != NSAPI_ERROR_OK) {
            printf("Could not connect! Returned %d\n", ret);
            return -1;
        }
    }
    printf("Connection established.\r\n");
    printf("\r\n");

    printf("MQTT client is trying to connect the server ...\r\n");
    {
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.MQTTVersion = 3;
        data.clientID.cstring = (char *)MQTT_CLIENT_ID;
        data.username.cstring = (char *)MQTT_USERNAME;
        data.password.cstring = (char *)MQTT_PASSWORD;

        mqttClient = new MQTTClient(socket);
        int rc = mqttClient->connect(data);
        if (rc != MQTT::SUCCESS) {
            printf("ERROR: rc from MQTT connect is %d\r\n", rc);
            return -1;
        }
    }
    printf("Client connected.\r\n");
    printf("\r\n");


    printf("Client is trying to subscribe a topic \"%s\".\r\n", MQTT_TOPIC_SUB);
    {
        int rc = mqttClient->subscribe(MQTT_TOPIC_SUB, MQTT::QOS0, messageArrived);
        if (rc != MQTT::SUCCESS) {
            printf("ERROR: rc from MQTT subscribe is %d\r\n", rc);
            return -1;
        }
        isSubscribed = true;
    }
    printf("Client has subscribed a topic \"%s\".\r\n", MQTT_TOPIC_SUB);
    printf("\r\n");

    tempControlThread.start(tempControl);
    groundControlThread.start(groundControl);
    ledPanelControlThread.start(ledPanelControl);

    // Enable UserBtn
    InterruptIn btn1(D5);
    btn1.rise(btn1_rise_handler);

    // Turn off the LED to let users know connection process done.
    led = LED_OFF;

    while(1) {
            /* Check connection */
        if(!mqttClient->isConnected()){
            break;
        }
        /* Pass control to other thread. */
        if(mqttClient->yield() != MQTT::SUCCESS) {
            break;
        }
        /* Received a control message. */
        if(isMessageArrived) {
            isMessageArrived = false;
            // Just print it out here.
            printf("\r\nMessage arrived:\r\n%s\r\n\r\n", messageBuffer);
        }
        
        bool sendLedMessage = LedMessage(&queueLed,&mpoolLed,mqttClient);
        if(sendLedMessage)
            continue;
        
        bool sendTempMessage = TempMessage(&queue, &mpool, mqttClient);
        if(sendLedMessage)
            continue;
        
        bool sendGroundMessage = GroundMessage(&queueGround, &mpoolGround, mqttClient);
        if(sendGroundMessage)
            continue;
    }
        printf("The client has disconnected.\r\n");

    if(mqttClient) {
        if(isSubscribed) {
            mqttClient->unsubscribe(MQTT_TOPIC_SUB);
        }
        if(mqttClient->isConnected()) 
            mqttClient->disconnect();
        delete mqttClient;
    }
    if(socket) {
        socket->close();
    }
    if(network) {
        network->disconnect();
      // network is not created by new.
    }
}
