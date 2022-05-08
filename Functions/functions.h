#ifndef FUNCTIONS
#define FUNCTIONS

#include "mbed.h"
#include <string>

void printTab(string name, double tab[], uint8_t num);
double movingAverage(double tab[]);
uint64_t upTime(uint8_t &hours,uint8_t &minutes,uint8_t &seconds);
#endif