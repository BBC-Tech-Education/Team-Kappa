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

        for (uint8_t i = 0; i < 12; i++) {
            Serial.printf("%d: %d  ", i, readings[i]);
        }
        Serial.println();


        // uint16_t red    = (readings[8] + readings[9])  / 2;
        // u_int16_t green = (readings[3] + readings[6])  / 2;
        // u_int16_t noise = (readings[0] + readings[11]) / 2;

        red_victim = 0;
        green_victim = 0;
        black_tile = 0;
        silver_tile = 0;

        if (readings[10] < 5000) {
            black_tile = 1;
        } else if (readings[6] < 12000) {
            if (readings[6] < 8000) {
                red_victim = 1;
            } else if (readings[1] > 5200) {
                red_victim = 1;
            } else if (readings[1] < 7000) {
                green_victim = 1;
            }
        } else if (readings[9] < 9000) {
            silver_tile = 1;
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