#include <Adafruit_BNO055.h>
#include "Motors.h"
#include <VL53L4CD.h>
#include <Wire.h>
#include "LRFs.h"
#include <Arduino_APDS9960.h>
#include "ColourSensor.h"

Motors motor;
LRFs lrfs;
RGBsensors ColourSensor(Wire2);

void setup() {
    ColourSensor.init();
}




void loop() {
    ColourSensor.update();

    if (ColourSensor.detect_green()) {
        Serial.println("Green works");
    } else if (ColourSensor.detect_red()) {
        Serial.println("Red Works.");
    } else {
        Serial.println("No victims detected.");
    }

    
}