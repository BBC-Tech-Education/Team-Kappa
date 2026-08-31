#ifndef COLOURSENSOR_H
#define COLOURSENSOR_H

#include <Arduino.h>
#include <Arduino_APDS9960.h>

class RGBsensors {
    public:
        RGBsensors(TwoWire &wire) {};
        void init();
        void update();
        bool detect_green();
        bool detect_red();

    private:
        APDS9960 coloursensor = APDS9960(Wire2,-1);;
        int16_t colours[3] = {0};
};

#endif
