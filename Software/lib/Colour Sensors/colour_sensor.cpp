#include "colour_sensor.h"






void ColourSensor::init()
{
    
    while (!sensor.begin(AS7341_I2CADDR_DEFAULT, &Wire2, 0)) {
        Serial.println("Could not find AS7341");
        delay(100);
    }


    // Increase the setASTEP number for more accuracy. However, this decreases
    // how fast it gives values (Default 999);

    sensor.setATIME(100);
    sensor.setASTEP(200);
    sensor.setGain(AS7341_GAIN_128X);

    sensor.setLEDCurrent(10);
    sensor.enableLED(true);

    sensor.startReading();

    green_victim = 0;
    red_victim = 0;

}

void ColourSensor::update()
{
    if (sensor.checkReadingProgress()) {
        sensor.getAllChannels(readings);

        // for (uint8_t i = 0; i < 12; i++) {
        // //     Serial.printf("%d: %d  ", i, readings[i]);
        // // }
        // // Serial.println();


        red_victim = 0;
        green_victim = 0;
        black_tile = 0;
        silver_tile = 0;



        if (readings[4] < 5000) {
            black_tile = 1;
            
        } else {
            float red_green = readings[8] / (float)readings[3];

            if (red_green > 3.0f) {
                red_victim = 1;
            } else if (red_green < 0.87f) {

                // Check to differentiate between green victim and fake sight.
                // delay(500);

                // if (readings[4] < 5000)
                //     black_tile = 1;
                // else {
                //     green_victim = 1;
                // }
                
                green_victim = 1;

            } else {
                float nir_blue = readings[5] / (float)readings[1];
                // Serial.print(nir_blue);
                if (nir_blue < 0.31f) {
                    silver_tile = 1;
                 }
            }
            // Serial.println();
        }

        sensor.startReading();

    

    }
}
    

uint8_t ColourSensor::detect_green()
{
    return green_victim;
}

uint8_t ColourSensor::detect_red()
{
    return red_victim;
}

uint8_t ColourSensor::detect_black()
{
    return black_tile;

}

uint8_t ColourSensor::detect_silver()
{
    return silver_tile;
}