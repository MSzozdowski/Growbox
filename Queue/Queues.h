#ifndef QUEUES
#define QUEUES

#include "mbed.h"
typedef struct {
    double    sensInTemp;   
    double    sensInHum;   
    double    sensOutTemp;   
    double    sensOutHum;
    bool      fansState;
    uint64_t  messageTime;
} message_t;

typedef struct {
    double ledPanelState;
    double fanMixState;
    double panelStart;
    double growDuration;
    uint64_t messageTime;
} message_l;

typedef struct {
    double usedMilliters;
    double totalMilliters;
    double voltage;
    uint64_t pumpingTime;
    uint64_t messageTime;
} message_g;

void tempControlQueue(double tempIn,double humIn, double tempOut, double humOut, bool fansSta, uint64_t t, Queue<message_t, 16> *queue, MemoryPool<message_t, 16> *mpool);
void ledControlQueue(double ledPanSt, double fanM, double panSt, double growDur,uint64_t t, Queue<message_l, 16> *queueLed, MemoryPool<message_l, 16> *mpoolLed);
void GroundControlQueue(double usedMil, double totalMil, double vol, uint64_t pumpT, uint64_t t, MemoryPool<message_g, 16> *mpoolGround, Queue<message_g,16> *queueGround);

#endif