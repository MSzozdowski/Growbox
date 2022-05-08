#include "Dht21.h"
#include <chrono>

Dht21::Dht21(PinName const &p) : _pin(p) {    
    _timer.start();
    _temperature = 0;
    _humidity = 0;
}

int Dht21::read()
{
    uint8_t bits[5] = {0};
    uint8_t cnt = 7;
    uint8_t idx = 0;

    // EMPTY BUFFER
    //for (int i=0; i< 5; i++) bits[i] = 0;
    //while(_timer.read_ms() < 10000) {}
    //_timer.stop();
    // Init
    _pin.output();
    _pin = 0;
    wait_us(1500);
    _pin = 1;
    wait_us(40);
    _pin.input();
    _pin.mode(PullUp);
    // ACKNOWLEDGE or TIMEOUT
    unsigned int loopCnt = 10000;
    while(_pin == 0) 
        if (loopCnt-- == 0) 
        {
            printf("DHTLIB_ERROR_TIME_OUT 1\n");
            return DHTLIB_ERROR_TIMEOUT;
        }
    
        

    loopCnt = 10000;
    while(_pin == 1) 
        if (loopCnt-- == 0)
        {
            printf("DHTLIB_ERROR_TIME_OUT 2\n");
            return DHTLIB_ERROR_TIMEOUT;
        } 
    
        

    // READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
    for (int i=0; i<40; i++)
    {
        loopCnt = 10000;
        while(_pin == 0) 
            if (loopCnt-- == 0) 
            {
                printf("DHTLIB_ERROR_TIME_OUT 3\n");
                return DHTLIB_ERROR_TIMEOUT;   
            }
        
            

        Timer t;
        t.start();

        loopCnt = 100000;
        while(_pin == 1) {
            if (loopCnt-- == 0) 
            {
                //printf("DHTLIB_ERROR_TIME_OUT 4\n");
                return DHTLIB_ERROR_TIMEOUT;
            }
        }
            

        if (t.read_us() > 40) //t.elapsed_time().count()
            bits[idx] |= (1 << cnt);
        if (cnt == 0)   // next byte?
        {
            cnt = 7;    // restart at MSB
            idx++;      // next byte!
        }
        else cnt--;
    }

    
    
    uint8_t dht21_check_sum;
    dht21_check_sum = bits[0] + bits[1] + bits[2] + bits[3];
    dht21_check_sum= dht21_check_sum%256;
    if (dht21_check_sum == bits[4]) {
            _humidity=(bits[0]*256+bits[1])*0.1f;
            _temperature=(bits[2]*256+bits[3])*0.1f;
        return DHTLIB_OK;
    } else {
        //printf("Incorrect checksum check_sum=%d bits[4]=%d\n",dht21_check_sum,bits[4]);
        return DHTLIB_ERROR_CHECKSUM;
    }
}
/*int Dht21::read(){
    Timer timer;
    uint8_t data[5] = {0};
    _pin.output();
    _pin  = 0;
    wait_us(1100);
    _pin = 1;
    wait_us(30);
    _pin.input();

    timer.start();
    while(timer.read_us() < 20) {
        if(_pin.read() == 0)
            break;
        else {
            printf("No reponse signal \n");
            return DHTLIB_ERROR_TIMEOUT;
        } 
    }
    timer.reset();

    while(_pin.read() == 0){
        if(timer.read_us() > 90){
            printf("1st response signal too long \n");
            return DHTLIB_ERROR_TIMEOUT;
        }
    }
    timer.reset();

    while(_pin.read() == 1){
        if(timer.read_us() > 90){
            printf("2nd response signal too long \n");
            return DHTLIB_ERROR_TIMEOUT;
        }
    }

	for (uint8_t j = 0; j < 5; j++) {
		data[j] = 0;
		for (uint8_t i = 8; i > 0; i--) {
			timer.reset();
			while (_pin.read() == 0) {
				if (timer.read_us() > 70) {
                    printf("Invalid first interval of a bit \n");
                    return DHTLIB_ERROR_TIMEOUT;
                }
			}

            timer.reset();
            while(_pin.read() == 1) {
                if(timer.read_us() > 90) {
                    printf("Invalid second interval of a bit \n");
                    return DHTLIB_ERROR_TIMEOUT;
                }
            }
            uint8_t bitTime = timer.read_us();
            
            if(bitTime < 50)
            {
                //read 0 - do nothing
            } else {
                data[j] |= 1 << (i - 1); //read 1 - mask a bit
            }
		}

	}
    uint8_t dht21_check_sum;
    dht21_check_sum = data[0] + data[1] + data[2] + data[3];
    dht21_check_sum= dht21_check_sum%256;
    if (dht21_check_sum == data[4]) {
            _humidity=(data[0]*256+data[1])*0.1f;
            _temperature=(data[2]*256+data[3])*0.1f;
        return DHTLIB_OK;
    } else {
        printf("Incorrect checksum check_sum=%d bits[4]=%d\n",dht21_check_sum,data[4]);
        return DHTLIB_ERROR_CHECKSUM;
    }
}*/

float Dht21::getCelsius() {
    return(_temperature);
}

float Dht21::getHumidity() {
    return(_humidity);
}
