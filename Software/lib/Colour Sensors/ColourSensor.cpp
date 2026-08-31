#include "ColourSensor.h"



void RGBsensors::init()
{
    Serial.begin(9600);
    while (!Serial);
    if (!coloursensor.begin()) {
        Serial4.println("Error initializing APDS-9960 sensor.");
  }
}

void RGBsensors::update() {

    while (!coloursensor.colorAvailable()) {
        delay(5);
    }

    int r, g, b;

    coloursensor.readColor(r, g, b);
    colours[0] = r;
    colours[1] = g;
    colours[2] = b;
}

bool RGBsensors::detect_green() {
    
    if ( colours[1] > colours[0] && (colours[1] - colours[2]) > 4) {
        return true;
    }

    return false;
}

bool RGBsensors::detect_red() {

    if (colours[0] > colours[1] && (colours[1] - colours[2]) < 2) {
        return true;
    }

    return false;
    
}