#include "LRFs.h"

//check continuity for the xshut - All are continuous
#define I2C_CLOCK 100000


void LRFs::init()
{   
    Wire.begin();
    Wire.setClock(I2C_CLOCK);
    
    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        pinMode(xshut_values[i], OUTPUT);
        digitalWrite(xshut_values[i], LOW);
    }

    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        pinMode(xshut_values[i], INPUT);
        delay(10);

        sensors[i].setTimeout(500);

        uint8_t attempts = 0;

        while(!sensors[i].init() && attempts < 10) {
            Serial.print("Failed to detect and initialize sensor ");
            Serial.println(i);
            delay(100);
            attempts ++;
        }
        sensors[i].setAddress(0x2A + i);
        sensors[i].startContinuous();
    }
}

void LRFs::update()
{
    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        if (sensors[i].dataReady()) {
            lrf_values[i] = sensors[i].read(false);
        }
        if (sensors[i].timeoutOccurred()) {
            lrf_values[i] = 0;
        }
    }
}

u_int16_t LRFs::get_value(int i)
{
    return lrf_values[i];
}