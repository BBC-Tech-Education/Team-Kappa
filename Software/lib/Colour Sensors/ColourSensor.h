#ifndef COLOURSENSOR_H
#define COLOURSENSOR_H

#include <Arduino.h>
#include <Arduino_APDS9960.h>

class RGBsensors {
    public:
        RGBsensors(TwoWire &wire);
        void init();
        void update();
        void detect_green();
        void detect_red();

    private:
        APDS9960 coloursensor;
        int16_t colours[3] = {0};
};

#endif
