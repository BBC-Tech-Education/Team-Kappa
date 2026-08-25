#ifndef LRFS_H
#define LRFS_H

#include <Wire.h>
#include <Pins.h>
#include <VL53L4CD.h>

#define SENSOR_NUM 8


class LRFs {
    public:
        LRFs() {};
        void init();
        void update();
    
    private:
        //create an array to store all the pins of the lrfs so i can loop for initialization.
        VL53L4CD sensors[SENSOR_NUM];
        uint16_t xshut_values[SENSOR_NUM] = {TOF_XSHUT0, TOF_XSHUT1, TOF_XSHUT2, TOF_XSHUT3, TOF_XSHUT4, TOF_XSHUT5, TOF_XSHUT6, TOF_XSHUT7};
        uint16_t lrf_values[SENSOR_NUM] = {0};


};


#endif