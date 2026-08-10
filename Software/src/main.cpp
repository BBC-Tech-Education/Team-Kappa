#include <Adafruit_BNO055.h>
#include "Motors.h"

Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO055_ADDRESS_B, &Wire);
Motors motor;


void setup() {
    delay(5000);
//  Serial.begin(115200);
    while(!bno.begin(OPERATION_MODE_IMUPLUS)) {
        Serial.println("No BNO055 detected. Check your wiring or I2C ADDR.");
        delay(1000);
    }
    delay(500);
    bno.setExtCrystalUse(true);
    delay(500);

    
 //bno.printSensorDetails();
    //motor.init();
   
}


void loop() {
    sensors_event_t event;
    bno.getEvent(&event);
    float bearing = event.orientation.x;

    Serial.print("Bearing: ");
    Serial.println(bearing);
   // motor.move(100.0f, -100.0f);


}