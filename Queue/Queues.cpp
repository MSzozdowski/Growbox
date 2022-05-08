#include "Queues.h"
void tempControlQueue(double tempIn,double humIn, double tempOut, double humOut,bool fansSta, uint64_t t, Queue<message_t, 16> *queue, MemoryPool<message_t, 16> *mpool){
    if(!queue->full()){
        message_t *que_message = mpool->alloc();
        que_message->sensInTemp = tempIn;
        que_message->sensInHum = humIn;
        que_message->sensOutTemp = tempOut;
        que_message->sensOutHum = humOut;
        que_message->fansState = fansSta;
        que_message->messageTime=t;
        queue->put(que_message);
    } else {
        printf("Temp control queue is full \n");
    }
}

void ledControlQueue(double ledPanSt, double fanM, double panSt, double growDur,uint64_t t, Queue<message_l, 16> *queueLed, MemoryPool<message_l, 16> *mpoolLed){
    if(!queueLed->full()){
        message_l *que_messageLed = mpoolLed->alloc();
        que_messageLed->ledPanelState=ledPanSt;
        que_messageLed->fanMixState=fanM;
        que_messageLed->panelStart=panSt;
        que_messageLed->growDuration=growDur;
        que_messageLed->messageTime=t;
        queueLed->put(que_messageLed);
    }  else {
        printf("Led control queue is full \n");
    }
}

void GroundControlQueue(double usedMil, double totalMil, double vol, uint64_t pumpT, uint64_t t, MemoryPool<message_g, 16> *mpoolGround, Queue<message_g,16> *queueGround){
    if(!queueGround->full()){
        message_g *que_messageGround = mpoolGround->alloc();
        que_messageGround->usedMilliters=usedMil;
        que_messageGround->totalMilliters=totalMil;
        que_messageGround->voltage=vol;
        que_messageGround->pumpingTime=pumpT;
        que_messageGround->messageTime=t;
        queueGround->put(que_messageGround);
    }else{
        printf("Ground control queue is full \n");
    }
}