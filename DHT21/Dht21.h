#ifndef DHT21_H
#define DHT21_H

#include "mbed.h"

#define DHTLIB_OK                0
#define DHTLIB_ERROR_CHECKSUM    1
#define DHTLIB_ERROR_TIMEOUT     2

class Dht21
{
public:
    Dht21(PinName const &p);
    int read();
    float getCelsius();
    float getHumidity();

private:
    float _humidity;
    float _temperature;
    DigitalInOut _pin;
    Timer _timer;
};

#endif
