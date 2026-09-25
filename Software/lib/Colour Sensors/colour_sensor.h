#ifndef COLOURSENSOR_H
#define COLOURSENSOR_H

#include <Arduino.h>
#include <Adafruit_AS7341.h>

class ColourSensor {
    public:
        ColourSensor() {};
        void init();
        void update();
        uint8_t detect_green();
        uint8_t detect_red();
        uint8_t detect_black();
        uint8_t detect_silver();


    private:
        Adafruit_AS7341 sensor;

        uint8_t still_reading;
        uint16_t readings[12];

        uint8_t green_victim;
        uint8_t red_victim;
        uint8_t black_tile;
        uint8_t silver_tile;

        int16_t colours[3] = {0};
};

#endif
