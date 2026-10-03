#include "LRFs.h"

//check continuity for the xshut - All are continuous
#define I2C_CLOCK 400000


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
    Serial.print("LRF Values: ");  
    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        if (sensors[i].dataReady()) {
            int16_t val = sensors[i].read(false);


            // sort this out. we see values less than 100 and 1200.
            if (val == 0 || val > 1200) {
                lrf_values[i] = 1200;
            } else if (val < 10) { 
                lrf_values[i] = 0;
            } else {
                lrf_values[i] = val;
            }
        }

        if (sensors[i].timeoutOccurred()) {
            lrf_values[i] = 0;
        }
        Serial.print(lrf_values[i]);
        Serial.print("\t");
    }
    Serial.println();

}

u_int16_t LRFs::get_value(int i)
{
    return lrf_values[i];
}