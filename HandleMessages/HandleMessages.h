#ifndef HANDLE_MESSAGES
#define HANDLE_MESSAGES
#include "Queues.h"


#include "mbed.h"
#include "MQTTClientMbedOs.h"

bool TempMessage(Queue<message_t, 16> *queue, MemoryPool<message_t, 16> *mpool,MQTTClient *mqttClient);
bool LedMessage(Queue<message_l, 16> *queueLed, MemoryPool<message_l, 16> *mpoolLed, MQTTClient *mqttClient);
bool GroundMessage(Queue<message_g,16> *queueGround, MemoryPool<message_g, 16> *mpoolGround, MQTTClient *mqttClient);
#endif