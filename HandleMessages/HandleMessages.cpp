#include "HandleMessages.h"
bool TempMessage(Queue<message_t, 16> *queue, MemoryPool<message_t, 16> *mpool,MQTTClient *mqttClient){
        if(!queue->empty()){
            osEvent evt = queue->get();
        if(evt.status == osEventMessage) { 
            message_t *que_message = (message_t *)evt.value.p;
            mpool->free(que_message);
            static unsigned short id = 0;
            static unsigned int count = 0;

            count++;

            // When sending a message, LED lights blue.
            //led = LED_ON;

            MQTT::Message message;
            message.retained = false;
            message.dup = false;

            const size_t buf_size = 150;
            char *buf = new char[buf_size];
            message.payload = (void*)buf;

            message.qos = MQTT::QOS0;
            message.id = id++;
            
            int ret = snprintf(buf, buf_size, "{\"message\":\"TempHumTask\",\"TempIn\": %.2f,\"HumIn\":%.2f,\"TempOut\":%.2f,\"HumOut\":%.2f,\"Fans\":%d,\"Time\":%llu}",
            que_message->sensInTemp,que_message->sensInHum,que_message->sensOutTemp,que_message->sensOutHum,que_message->fansState,que_message->messageTime); //"Nacisnieto przycisk po raz %d" count
            if(ret < 0) {
                printf("ERROR: snprintf() returns %d.", ret);
                return 1;
            }
            message.payloadlen = ret;
            // Publish a message.
            //printf("Publishing %d message.\r\n",count);
            int rc = mqttClient->publish("topic", message);
            if(rc != MQTT::SUCCESS) {
                printf("ERROR: rc from MQTT publish is %d\r\n", rc);
            }
            printf("Message %d published.\r\n",count);
            delete[] buf;    

            thread_sleep_for(200);
            //led = LED_OFF;
        } else {
            printf("Can not get message from queue \n");     
        }
        }
        return 0;
}

bool LedMessage(Queue<message_l, 16> *queueLed, MemoryPool<message_l, 16> *mpoolLed, MQTTClient* mqttClient){
        if(!queueLed->empty()){
            osEvent evtLed = queueLed->get();
        if(evtLed.status == osEventMessage) { 
            message_l *que_messageLed = (message_l *)evtLed.value.p;
            mpoolLed->free(que_messageLed);
            static unsigned short id = 0;
            static unsigned int count = 0;

            count++;

            // When sending a message, LED lights blue.
            //led = LED_ON;

            MQTT::Message message;
            message.retained = false;
            message.dup = false;

            const size_t buf_size = 150;
            char *buf = new char[buf_size];
            message.payload = (void*)buf;

            message.qos = MQTT::QOS0;
            message.id = id++;
            int ret = snprintf(buf, buf_size, "{\"message\":\"LedTask\", \"PanelState\" : %d, \"FanMixState\": %d, \"PanelStart\": %d, \"GrowTime\": %d, \"Time\": %llu }"
            ,(bool)que_messageLed->ledPanelState,(bool)que_messageLed->fanMixState,(uint8_t)que_messageLed->panelStart,(uint8_t)que_messageLed->growDuration,(uint64_t)que_messageLed->messageTime);
            if(ret < 0) {
                printf("ERROR: snprintf() returns %d.", ret);
                return 1;
            }
            message.payloadlen = ret;
            // Publish a message.
            //printf("Publishing %d message.\r\n",count);
            int rc = mqttClient->publish("topic", message);
            if(rc != MQTT::SUCCESS) {
                printf("ERROR: rc from MQTT publish is %d\r\n", rc);
            }
            printf("Message %d published.\r\n",count);
            delete[] buf;    
     
            thread_sleep_for(200);
            //led = LED_OFF;
        } else {
            printf("Can not get message from queue \n");     
        }
        }
        return 0;
}

bool GroundMessage(Queue<message_g, 16> *queueGround, MemoryPool<message_g, 16> *mpoolGround, MQTTClient *mqttClient){
        if(!queueGround->empty()){
            osEvent evtGround = queueGround->get();
        if(evtGround.status == osEventMessage) { 
            message_g *que_messageGround = (message_g *)evtGround.value.p;
            mpoolGround->free(que_messageGround);
            static unsigned short id = 0;
            static unsigned int count = 0;
            count++;
            // When sending a message, LED lights blue.
            //led = LED_ON;
            MQTT::Message message;
            message.retained = false;
            message.dup = false;
            const size_t buf_size = 150;
            char *buf = new char[buf_size];
            message.payload = (void*)buf;
            message.qos = MQTT::QOS0;
            message.id = id++;
            int ret = snprintf(buf, buf_size, "{\"message\":\"GroundTask\", \"UsedMilliliters\" : %.2f, \"TotalMililiters\": %.2f, \"Voltage\": %.2f, \"PumpingTime\": %llu, \"Time\": %llu }"
            ,que_messageGround->usedMilliters,que_messageGround->totalMilliters,que_messageGround->voltage,que_messageGround->pumpingTime,que_messageGround->messageTime);
            if(ret < 0) {
                printf("ERROR: snprintf() returns %d.", ret);
                return 1;
            }
            message.payloadlen = ret;
            // Publish a message.
            //printf("Publishing %d message.\r\n",count);
            int rc = mqttClient->publish("topic", message);
            if(rc != MQTT::SUCCESS) {
               printf("ERROR: rc from MQTT publish is %d\r\n", rc);
            }
            printf("Message %d published.\r\n",count);
            delete[] buf;   
            thread_sleep_for(200);
            //led = LED_OFF;
        } else {
            printf("Can not get message from queue \n");     
        }
        }
        return 0;
}