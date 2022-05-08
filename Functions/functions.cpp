#include "functions.h"

void printTab(string name, double tab[], uint8_t num){
    uint8_t tabSize = 5;
    printf("%s \t",name.c_str());
    for(int i=0;i<tabSize;i++){
        if(i==num)
        printf("->%.2f \t",tab[i]);
        else
        printf("%.2f \t",tab[i]);
    }
    printf("\n");
}

double movingAverage(double tab[]){
    double averageVal = 0.0;
    uint8_t tabSize = 5;
    for(int i=0;i<tabSize;i++){
        averageVal+=tab[i];
        if(!tab[i])
            return 0.0;
    }
    return averageVal/tabSize;
}

uint64_t upTime(uint8_t &hours,uint8_t &minutes,uint8_t &seconds){
    time_t t = time(NULL);
    struct tm *tmp = gmtime(&t);
    hours = (t / 3600) % 24;  
    minutes = (t / 60) % 60;
    seconds = t % 60;
    return t;
}
