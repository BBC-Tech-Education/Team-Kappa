#include <Adafruit_BNO055.h>
#include "Motors.h"
#include <VL53L4CD.h>
#include <Wire.h>
#include "LRFs.h"
#include <Arduino_APDS9960.h>

Motors motor;
LRFs lrfs;
APDS9960 ColourSensor(Wire2,-1);

void setup() {
    // lrfs.init();
    // motor.init();

   Serial.begin(9600);
   while (!Serial);
   if (!ColourSensor.begin()) {
        Serial4.println("Error initializing APDS-9960 sensor.");
  }
}

// void movefwd(int tagDis) {
//     while (lrf.read(0) > tagDis) {
//         motor.move(20.0f, 20.0f);
//     }
  
// }



void loop() {
    // if(done == false) {
    //     motor.move(20.0f, 20.0f);
    //     delay(4000);
    //     motor.move(-20.0f, -20.0f);
    //     delay (4000);
    //     done = true;
    // } else {
    //     motor.move(0.0f, 0.0f);
    //     done = true;
    //    lrfs.update();
    while (! ColourSensor.colorAvailable()) {
        delay(5);
    }

    int r, g, b;

    // read the color
    ColourSensor.readColor(r, g, b);

    // print the values
    Serial.print("r = ");
    Serial.println(r);
    Serial.print("g = ");
    Serial.println(g);
    Serial.print("b = ");
    Serial.println(b);
    Serial.println();

    // wait a bit before reading again
    delay(20);
}