# ifndef LRF_H
#define LRF_H

#include <vl53l4cx_class.h>
#include <Arduino.h>
#include <Wire.h>
#include <Pins.h>

class LRF {
    public:
        LRF(){};
        void ini();
        void read_all();
        double lrf_values [8] = {0};
    
    private:
        //create an array to store all the pins of the lrfs so i can loop for initialization.

};


#endif